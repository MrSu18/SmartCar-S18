/*
 * ImageProcess.c
 *
 *  Created on: 2023年3月1日
 *      Author: L
 */
#include "ImageProcess.h"
#include "zf_device_tft180.h"
#include "zf_driver_pit.h"
#include "ImageProcess.h"
#include "ImageConversion.h"
#include "ImageBasic.h"
#include "ImageTrack.h"
#include "math.h"
#include "ImageSpecial.h"
#include <string.h>
#include "pid.h"
#include "Control.h"
#include "debug.h"
#include "icm20602.h"
#include "motor.h"

//内轮纯减速可以跑72环岛70环内加速到75，入库前降速到65，转向P为12D为1
/*1:左环岛 2:右环岛 3:十字 4:断路 5:坡道 6:路障 7:入左库 8:入右库 9:库直行 'S':停车 'E':编码器测距1m 'G':陀螺仪积分70°*/
uint8 process_status[PROCESS_LENGTH]={3,3,'S',3,3,3,3,5,2,1,3,4,6,5,3,3,3,7,'S'};//总状态机元素执行顺序数组
uint16 process_speed[PROCESS_LENGTH]={66,66,66,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64};//上面数组对应的元素路段的速度
ProcessProperty process_property[PROCESS_LENGTH];
uint8 process_status_cnt=0;//元素状态数组的计数器

/***********************************************
* @brief : 图像处理和元素识别
* @param : 无
* @return: 无
* @date  : 2023.3.6
* @author: 刘骏帆
************************************************/
void ImageProcess(void)
{
    static uint8 encoder_status=0,gyro_status=0;;
    //扫线
    EdgeDetection();
    //边线进行透视
    EdgeLinePerspective(left_line,l_line_count,per_left_line);
    EdgeLinePerspective(right_line,r_line_count,per_right_line);
    //对边线进行滤波
    BlurPoints(per_left_line, l_line_count, f_left_line, LINE_BLUR_KERNEL);
    BlurPoints(per_right_line, r_line_count, f_right_line, LINE_BLUR_KERNEL);
    //等距采样，由于我是对透视图像进行扫线所以不需要进行等距采样
    ResamplePoints(f_left_line, l_line_count, f_left_line1, &per_l_line_count, SAMPLE_DIST*PIXEL_PER_METER);
    ResamplePoints(f_right_line, r_line_count, f_right_line1, &per_r_line_count, SAMPLE_DIST*PIXEL_PER_METER);
    //局部曲率
    local_angle_points(f_left_line1,per_l_line_count,l_angle,ANGLE_DIST/SAMPLE_DIST);
    local_angle_points(f_right_line1,per_r_line_count,r_angle,ANGLE_DIST/SAMPLE_DIST);
    //曲率极大值抑制
    nms_angle(l_angle,per_l_line_count,l_angle_1,(ANGLE_DIST/SAMPLE_DIST)*2+1);
    nms_angle(r_angle,per_r_line_count,r_angle_1,(ANGLE_DIST/SAMPLE_DIST)*2+1);
    //跟踪左线
    track_leftline(f_left_line1, per_l_line_count, center_line_l, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
    track_rightline(f_right_line1, per_r_line_count , center_line_r, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));

    //切换左右巡线
    if(per_r_line_count > 2 && per_l_line_count < 20)
    {
        track_type=kTrackRight;
    }
    else if(per_l_line_count>2 && per_r_line_count < 20)
    {
        track_type=kTrackLeft;
    }
    else if(per_l_line_count < 2 && per_r_line_count > 2)
    {
        track_type = kTrackRight;
    }
    else if(per_l_line_count > 2 && per_r_line_count < 2)
    {
        track_type=kTrackLeft;
    }
    else
    {
        track_type=kTrackRight;
    }

#if 1
    switch(process_status[process_status_cnt])
    {
        case 1://左环岛
            if(CircleIslandLStatus()==1)
            {
                process_status_cnt++;
                original_speed=process_speed[process_status_cnt];
                base_speed=original_speed;
            }
            break;
        case 2://右环岛
            if(CircleIslandRStatus()==1)
            {
                process_status_cnt++;
                original_speed=process_speed[process_status_cnt];
                base_speed=original_speed;
            }
            break;
        case 3://十字
            if(CrossIdentify()==1)
            {
                process_status_cnt++;
                original_speed=process_speed[process_status_cnt];
                base_speed=original_speed;
            }
            break;
        case 4://断路
            if(CutIdentify()==1)
            {
                process_status_cnt++;
                original_speed=process_speed[process_status_cnt];
                base_speed=original_speed;
            }
            break;
        case 5://坡道
            if(SlopeIdentify()==1)
            {
                process_status_cnt++;
                original_speed=process_speed[process_status_cnt];
                base_speed=original_speed;
            }
            break;
        case 6://路障
            if(BarrierIdentify()==1)
            {
                process_status_cnt++;
                original_speed=process_speed[process_status_cnt];
                base_speed=original_speed;
            }
            break;
        case 7://左车库
            if(GarageIdentify_L()==1)
            {
                gpio_toggle_level(BEER);
            }
            break;
        case 8://右车库
            if(GarageIdentify_R()==1)
            {
                gpio_toggle_level(BEER);
            }
            break;
        case 9://库直行
            if(GarageIdentify_Straight()==1)
            {
                gpio_toggle_level(BEER);
                process_status_cnt++;
                original_speed=process_speed[process_status_cnt];
                base_speed=original_speed;
            }
            break;
        case 'S':
            pit_disable(CCU60_CH1);
            pit_disable(CCU60_CH0);//关闭电机中断
            MotorSetPWM(0,0);
            break;
        case 'E':
            if(encoder_status==0)
            {
                encoder_status=1;
                encoder_dis_flag=1;//开启编码器测距
            }
            else
            {
                if(dis>1000)
                {
                    encoder_dis_flag=0;
                    encoder_status=0;
                    gpio_toggle_level(BEER);
                    process_status_cnt++;
                    original_speed=process_speed[process_status_cnt];
                    base_speed=original_speed;
                }
            }
            break;
        case 'G':
            if(gyro_status==0)
            {
                gyro_status=1;
                StartIntegralAngle_X(70);//开启陀螺仪积70度
            }
            else
            {
                if(icm_angle_x_flag==1)//积分到达
                {
                    gyro_status=0;
                    gpio_toggle_level(BEER);
                    process_status_cnt++;
                    original_speed=process_speed[process_status_cnt];
                    base_speed=original_speed;
                }
            }
            break;
        default:break;
    }
#endif
    //预瞄点求偏差
    if(track_type==kTrackRight)
    {
        if(r_line_count<10 && (process_status[process_status_cnt]==1 || process_status[process_status_cnt]==2))//你想让他寻右线但是右边线不存在时，右边重新扫线
        {
            RightLineDetectionAgain('y');
            EdgeLinePerspective(right_line,r_line_count,per_right_line);
            per_r_line_count=PER_EDGELINE_LENGTH;
            BlurPoints(per_right_line, r_line_count, f_right_line, LINE_BLUR_KERNEL);
            ResamplePoints(per_right_line, r_line_count, f_right_line1, &per_r_line_count, SAMPLE_DIST*PIXEL_PER_METER);
            track_rightline(f_right_line1, per_r_line_count , center_line_r, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
        }
        image_bias = GetAnchorPointBias(aim_distance, per_r_line_count, center_line_r);
    }
    else if(track_type==kTrackLeft)
    {
        if (l_line_count<10 && (process_status[process_status_cnt]==1 || process_status[process_status_cnt]==2))//你想让他寻左线但是左边线不存在时，左边重新扫线
        {
            LeftLineDetectionAgain('y');
            per_l_line_count=PER_EDGELINE_LENGTH;
            EdgeLinePerspective(left_line,l_line_count,per_left_line);
            BlurPoints(per_left_line, l_line_count, f_left_line, LINE_BLUR_KERNEL);
            ResamplePoints(f_left_line, l_line_count, f_left_line1, &per_l_line_count, SAMPLE_DIST*PIXEL_PER_METER);
            track_leftline(f_left_line1, per_l_line_count, center_line_l, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
        }
        image_bias = GetAnchorPointBias(aim_distance, per_l_line_count, center_line_l);
    }
    //速度决策
//    if(speed_type==kImageSpeed)
//    {
//        base_speed=SpeedDecision(original_speed,8);//弯道是68直道是80
//    }
//    tft180_show_uint(0, 0, base_speed, 3);
}

/***********************************************
* @brief : 赛道基础信息变量重置，为下一帧做准备
* @param : 全局变量
* @return: 初始化为0的全局变量
* @date  : 2023.3.6
* @author: 刘骏帆
* @note  :  无
************************************************/
void TrackBasicClear(void)
{
    //边线丢线数组清零
    l_line_count=0;r_line_count=0;//边线的计数指针清零
    per_l_line_count=PER_EDGELINE_LENGTH,per_r_line_count=PER_EDGELINE_LENGTH;
    l_lostline_num=0;r_lostline_num=0;//丢线数清零
    //生长趋势的数组清零
    memset(l_growth_direction,0,8);
    memset(r_growth_direction,0,8);
}

/***********************************************
* @brief : 出界保护程序，通过遍历图像最后一行的像素点
* @param : void
* @return: void
* @date  : 2023.3.1
* @author: L & 刘骏帆
************************************************/
void OutProtect(void)
{
    int over_count = 0;                                   //存放超过阈值的像素点的数量
    int adc_sum = 0;

    for(int16 i = 0;i < 5;i++)
        adc_sum += normalvalue[i];
    for(int16 i = 0;i < MT9V03X_W;i++)                       //遍历最后一行
    {
        if(mt9v03x_image[106][i] <= OUT_THRESHOLD)
                over_count++;
    }
    //如果全部超过阈值则停止
    if(over_count>=MT9V03X_W-2 && adc_sum < 10 && process_status[process_status_cnt]!=6)//路障的时候关闭出界保护
    {
        pit_disable(CCU60_CH0);//关闭电机中断
        pit_disable(CCU60_CH1);
        pit_disable(CCU61_CH1);
        MotorSetPWM(0,0);
    }
}

/***********************************************
* @brief : 状态机变量属性初始
* @param : void
* @return: void
* @date  : 2023.3.1
* @author: L & 刘骏帆
************************************************/
void ProcessPropertyInit(void)
{
    for(uint8 i=0;i<PROCESS_LENGTH;i++)
    {
        if(process_status[i]==7 || process_status[i]==8)//入库
        {
            process_property[i].min_speed=60;
        }
        switch(process_status[i])
        {
            case 1://左环岛
                process_property[i].max_speed=65;
                process_property[i].min_speed=60;
                break;
            case 2://右环岛
                process_property[i].max_speed=60;
                process_property[i].min_speed=60;
                break;
            case 4://断路
                process_property[i].max_speed=60;
                process_property[i].min_speed=62;
                break;
            case 5://坡道
                process_property[i].min_speed=60;
                break;
            case 6://路障
                process_property[i].min_speed=60;
                break;
            default:break;
        }
    }
}
