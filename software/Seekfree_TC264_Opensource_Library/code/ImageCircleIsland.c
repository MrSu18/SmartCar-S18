//
// Created by 30516 on 2023/3/10.
//
#include "ImageSpecial.h"
#include "ImageTrack.h"
#include <stdio.h>
#include "zf_device_tft180.h"
#include "adc.h"
#include "stdlib.h"
#include "debug.h"
#include "motor.h"
#include "icm20602.h"
#include "Control.h"
#include "zf_driver_gpio.h"

//#define PI 3.1415926
#define CIRCLE_SIDE_LARGE_ADC_THR 2000 //车到了环岛中下部时靠近环岛那边边ADC的值应该大于这个阈值
#define CIRCLE_SIDE_SMALL_ADC_THR 1000 //车到了环岛中下部时另外一边ADC的值应该小于于这个阈值
#define CIRCLE_MID_ADC_THR        3000 //车到了环岛中下部时，中间电感应该大于这个阈值
#define CIRCLE_SPECIAL_ADC_THR    4095 //车子由于路径完全平行远离环岛，这时候双边电感会等于4095并且车在环岛中部需要及时入环
#define CIRCLE_SPECIAL_ADC_THR2   4000 //车子由于路径完全平行远离环岛，这时候靠近环侧内八会大于3000并且车在环岛中部需要及时入环

/**************************************************左环岛***************************************************************/
uint8 CircleIslandLStatus()//左边环岛状态状态机
{
    static uint8 status=0;//环岛状态转移变量
//    tft180_show_uint(120, 100, status, 2);
    switch (status)
    {
        case 0: //检测左环岛
            if(CircleIslandLDetection()==1)
            {
                speed_type=kNormalSpeed;//关闭速度决策
                base_speed=60;//降速进环
                gpio_set_level(BEER,1);
                status=1;
            }
            else if(L>=CIRCLE_SPECIAL_ADC_THR && LM>CIRCLE_SPECIAL_ADC_THR2)//避免由于车子是平行偏离环岛的特殊电磁情况，这时候车子大概在环岛中部已经需要入环了所以跳过状态1
            {
                speed_type=kNormalSpeed;//关闭速度决策
                base_speed=60;//降速进环
                gpio_set_level(BEER,1);
                StartIntegralAngle_X(320);//开启陀螺仪准备积分出环
                status=2;
            }
            break;
        case 1: //路过环岛第一个入口
            if(CircleIslandLInDetection()==1)
            {
                StartIntegralAngle_X(320);//开启陀螺仪准备积分出环
                status=2;
            }
            break;
        case 2: //进入环岛
            if(CircleIslandLIn()==1)
            {
//                base_speed=65;//环内加速
                status=3;
            }
            else  if (CircleIslandLOutFinish()==1)//防止太切内而看不到外环使得状态错乱，陀螺仪积分到了则强制出环
            {
                status=5;
            }
            break;
        case 3: //检测出环
            track_type=kTrackRight;//在环内默认寻外圆
            if(CircleIslandLOutDetection()==1)
            {
                track_type=kTrackRight;
                status=4;
            }
            else  if (CircleIslandLOutFinish()==1)//防止太切内而看不到外环使得状态错乱，陀螺仪积分到了则强制出环
            {
                status=5;
            }
            break;
        case 4://出环
            if (CircleIslandLOutFinish()==1)
            {
                base_speed=original_speed;//恢复速度
                status=5;
            }
            else CircleIslandLOut();//出环没结束就一直做出环处理
            break;
        case 5://检测环岛是否结束
            if (CircleIslandLEnd()==1)
            {
                base_speed=original_speed;//恢复速度
                speed_type=kImageSpeed;//开启速度决策
                gpio_set_level(BEER,0);
                status=0;
                return 1;
            }
            break;
        default:
            break;
    }
    return 0;
}

/***********************************************
* @brief : 检测小车是否走到了环岛元素部分
* @param : 无
* @return: 0：还没检测到左角点 1：检测到了左边角点 2：左角点消失
* @date  : 2023.5.26
* @author: 刘骏帆
* @notice: 角度./180.*PI，因为求得的曲率是弧度制的不是角度制
************************************************/
uint8 CircleIslandLDetection()//检测左环岛
{
    track_type=kTrackRight;//默认寻右边
    //环岛中下部分电磁特征
    if((L>CIRCLE_SIDE_LARGE_ADC_THR || R>CIRCLE_SIDE_LARGE_ADC_THR) && M>CIRCLE_MID_ADC_THR)
    {
        return 1;
    }
    return 0;
}

/***********************************************
* @brief : 检测是否到了需要入环的状态
* @param : 无
* @return: 是否入环 1：入环 0：入环还不用入环
* @date  : 2023.5.26
* @author: 刘骏帆
************************************************/
uint8 CircleIslandLInDetection(void)
{
    static uint8 status=0;
    if(status==0)
    {
        encoder_dis_flag = 1;//开启编码器计数
        status=1;
    }
    else if(status==1)
    {
        if(dis>150)//20cm
        {
            encoder_dis_flag = 0;
            status=0;
            track_type=kTrackLeft;
            return 1;
        }
    }
    track_type=kTrackRight;//否则还没到入环时机
    return 0;
}

/***********************************************
* @brief : 左环岛入环状态
* @param : 无
* @return: 是否入环结束 1：入环结束 0：入环未结束
* @date  : 2023.4.15
* @author: 刘骏帆
************************************************/
#define JUDGE_IN_EDD_THR    40//判断入环状态结束的右边线两端X坐标的差值阈值
uint8 CircleIslandLIn()//入环状态
{
    //这里是判断是否跳出入环状态
    int len=0;
    if (per_r_line_count>EDGELINE_LENGTH-10) len=EDGELINE_LENGTH-10;
    else                                     len=per_r_line_count;
    if(f_right_line1[0].X-f_right_line1[len-1].X>JUDGE_IN_EDD_THR && right_line[r_line_count-1].X<80)
    {
        return 1;//入环结束
    }
    //对入环状态进行偏差处理
    if(l_line_count<5)//左边90行都丢线，说明只能沿着外边进去
    {
        //重新扫线
        LeftLineDetectionAgain('n');
        //对重新扫出来的线的两端来对车体位置进行判断,然后对中线进行处理
        if(left_line[0].X<left_line[l_line_count-1].X && left_line[0].Y<100)//左边看不到圆环内环，只能看到圆环外环
        {
            uint8 y=0;//flag是连续变量
            r_line_count=1;
            for (uint8 i=0;i<l_line_count-2;i++)
            {
                if(left_line[l_line_count-1-i].Y>=y)
                {
                    y=left_line[l_line_count-1-i].Y;
                }
                else if (left_line[l_line_count-1-i].X>left_line[l_line_count-2-i].X)//在y开始回升的情况下下一个点的x还是在这个点的左边
                {
                    right_line[r_line_count]=left_line[l_line_count-i];
                    r_line_count++;
                }
            }
//            l_line_count=0;per_l_line_count=0;
            per_r_line_count=PER_EDGELINE_LENGTH;
            //对边线进行透视
            EdgeLinePerspective(right_line,r_line_count,per_right_line);
            BlurPoints(per_right_line, r_line_count, f_right_line, LINE_BLUR_KERNEL);
            ResamplePoints(f_right_line, r_line_count, f_right_line1, &per_r_line_count, SAMPLE_DIST*PIXEL_PER_METER);
            track_rightline(f_right_line1, per_r_line_count, center_line_r, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
            track_type=kTrackRight;
        }
        else track_type=kTrackLeft;//如果没丢线寻左边线的圆环进去
    }
    else
    {
        track_type=kTrackLeft;//如果没丢线寻左边线的圆环进去
    }
    return 0;
}

/***********************************************
* @brief : 左环岛检测出环
* @param : 无
* @return: 是否到出环状态 1：环岛出口 0：还在环中
* @date  : 2023.4.15
* @author: 刘骏帆
************************************************/
uint8 CircleIslandLOutDetection()//左环岛出环状态
{
    if (r_line_count<2)//右边一开始就丢线
    {
        //重新扫线
        RightLineDetectionAgain('n');
        per_r_line_count=PER_EDGELINE_LENGTH;
        EdgeLinePerspective(right_line,r_line_count,per_right_line);
        BlurPoints(per_right_line, r_line_count, f_right_line, LINE_BLUR_KERNEL);
        ResamplePoints(f_right_line, r_line_count, f_right_line1, &per_r_line_count, SAMPLE_DIST*PIXEL_PER_METER);
        local_angle_points(f_right_line1,per_r_line_count,r_angle,ANGLE_DIST/SAMPLE_DIST);
        nms_angle(r_angle,per_r_line_count,r_angle_1,(ANGLE_DIST/SAMPLE_DIST)*2+1);
        track_rightline(f_right_line1, per_r_line_count, center_line_r, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
    }
    //进行角点判断
    for (int i = 0; i < per_r_line_count; ++i)
    {
        if (70./180.*PI<r_angle_1[i] && r_angle_1[i]<140./180.*PI && i<0.8/SAMPLE_DIST)//出环右边角点
        {
            return 1;
        }
    }
    return 0;
}

/***********************************************
* @brief : 左环岛出环
* @param : 无
* @return: 0：出环未结束 1：出环结束
* @date  : 2023.4.16
* @author: 刘骏帆
************************************************/
uint8 CircleIslandLOutFinish(void)//检测环岛是否结束
{
    if(icm_angle_x_flag==1)
    {
        return 1;
    }
    return 0;
}
void CircleIslandLOut(void)//环岛出环处理函数
{
    //出环处理
    if (l_line_count>2)//左边如果还能看到线就沿着圆出去
    {
        track_type=kTrackLeft;
    }
    else
    {
        if (r_line_count>4)
        {
            //判断角点的位置
            for (int i = 0; i < per_r_line_count; ++i)
            {
                if (70./180.*PI<r_angle_1[i] && r_angle_1[i]<140./180.*PI)//出环右边角点
                {
                    if (i>aim_distance/SAMPLE_DIST)
                    {
                        per_r_line_count=i;
                        track_type=kTrackRight;
                    }
                    else//继承上次的偏差
                    {
                        if (image_bias<5) image_bias=8;//如果上次太小就固定打死
                        track_type=kTrackSpecial;
                    }
                    break;
                }
            }
        }
        else//左右边线都丢线
        {
            r_line_count=0;//重新规划右边线
            //找到补线的左上角点
            uint8 half=GRAY_BLOCK/2;
            myPoint left_seed=left_line[l_line_count-1];//左种子
            if (left_seed.Y==0)
            {
                left_seed.Y=USE_IMAGE_H_MAX-half-1,left_seed.X=half;
            }
            for (; left_seed.Y>USE_IMAGE_H_MIN; left_seed.Y--)
            {
                if(PointSobelTest(left_seed)==1) break;
            }
            myPoint right_seed;//右种子
            right_seed.Y=USE_IMAGE_H-half-1,right_seed.X=USE_IMAGE_W-half-2;//图像右下角
            right_line[r_line_count]=right_seed;r_line_count++;//顺序不能错，不然这条线的方向就反了
            right_line[r_line_count]=left_seed;r_line_count++;
            //*****************************
            per_r_line_count=PER_EDGELINE_LENGTH;
            EdgeLinePerspective(right_line,r_line_count,per_right_line);
            ResamplePoints(per_right_line, r_line_count, f_right_line1, &per_r_line_count, SAMPLE_DIST*PIXEL_PER_METER);
            track_rightline(f_right_line1, per_r_line_count, center_line_r, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
            track_type=kTrackRight;
        }
    }
}

/***********************************************
* @brief : 判断左环岛是否结束
* @param : 无
* @return: 无
* @date  : 2023.3.19
* @author: 刘骏帆
************************************************/
uint8 CircleIslandLEnd(void)
{
    if(l_line_count>10)//判断
    {
        if(left_line[10].X-left_line[0].X>5)//并且左边线有像右边延生的趋势，利用了梯形畸变近大远小
            return 1;
    }
    //循迹
    track_type=kTrackRight;
    return 0;
}
/**********************************************************************************************************************/

/**************************************************右环岛***************************************************************/
uint8 CircleIslandRStatus()//右边环岛状态状态机
{
    static uint8 status;//环岛状态转移变量
//    tft180_show_uint(120, 100, status, 2);
    switch (status)
    {
        case 0: //检测右环岛
            if(CircleIslandRDetection()==1)
            {
                speed_type=kNormalSpeed;//关闭速度决策
                base_speed=60;//降速进环
                gpio_set_level(BEER,1);//开启蜂鸣器
                status=1;
            }
            else if(R>=CIRCLE_SPECIAL_ADC_THR && RM>CIRCLE_SPECIAL_ADC_THR2)//避免由于车子是平行偏离环岛的特殊电磁情况，这时候车子大概在环岛中部已经需要入环了所以跳过状态1
            {
                speed_type=kNormalSpeed;//关闭速度决策
                base_speed=60;//降速进环
                gpio_set_level(BEER,1);
                StartIntegralAngle_X(320);//开启陀螺仪准备积分出环
                status=2;
            }
            break;
        case 1: //路过环岛第一个入口
            if(CircleIslandRInDetection()==1)
            {
                gpio_toggle_level(P21_3);
                StartIntegralAngle_X(320);//开启陀螺仪准备积分出环
                status=2;
            }
            break;
        case 2: //进入环岛
            if(CircleIslandRIn()==1)
            {
//                base_speed=65;
                status=3;
            }
            else  if (CircleIslandROutFinish()==1)//防止太切内而看不到外环使得状态错乱，陀螺仪积分到了则强制出环
            {
                status=5;
            }
            break;
        case 3: //检测出环
            track_type=kTrackLeft;//在环内默认寻外圆
            if(CircleIslandROutDetection()==1)
            {
                track_type=kTrackLeft;
                status=4;
            }
            else  if (CircleIslandROutFinish()==1)//防止太切内而看不到外环使得状态错乱，陀螺仪积分到了则强制出环
            {
                status=5;
            }
            break;
        case 4://出环
            if (CircleIslandROutFinish()==1)
            {
                base_speed=original_speed;//恢复速度
                status=5;
            }
            else CircleIslandROut();//出环没结束就一直做出环处理
            break;
        case 5://检测环岛是否结束
            if (CircleIslandREnd()==1)
            {
                base_speed=original_speed;//恢复速度
                speed_type=kImageSpeed;//启动速度决策
                gpio_set_level(BEER,0);
                status=0;
                return 1;
            }
            break;
        default:
            break;
    }
    return 0;
}

/***********************************************
* @brief : 检测小车是否走到了环岛元素部分
* @param : 无
* @return: 0：还没检测到左角点 1：检测到了左边角点 2：左角点消失
* @date  : 2023.5.26
* @author: 刘骏帆
* @notice: 角度./180.*PI，因为求得的曲率是弧度制的不是角度制
************************************************/
uint8 CircleIslandRDetection()//检测左环岛
{
    track_type=kTrackLeft;//默认寻左边
    //环岛中下部分电磁特征
    //左边大于阈值的时候是车体太靠近环内的情况
    if((L>CIRCLE_SIDE_LARGE_ADC_THR || R>CIRCLE_SIDE_LARGE_ADC_THR) && M>CIRCLE_MID_ADC_THR)
    {
        return 1;
    }
    return 0;
}

/***********************************************
* @brief : 检测是否到了需要入环的状态
* @param : 无
* @return: 是否入环 1：入环 0：入环还不用入环
* @date  : 2023.5.26
* @author: 刘骏帆
************************************************/
uint8 CircleIslandRInDetection(void)
{
    static uint8 status=0;
    if(status==0)
    {
        encoder_dis_flag = 1;//开启编码器计数
        status=1;
    }
    else if(status==1)
    {
        if(dis>150)//20cm
        {
            encoder_dis_flag = 0;
            status=0;
            track_type=kTrackRight;
            return 1;
        }
    }
    track_type=kTrackLeft;//否则还没到入环时机
    return 0;
}

/***********************************************
* @brief : 右环岛入环状态
* @param : 无
* @return: 是否入环结束 1：入环结束 0：入环未结束
* @date  : 2023.5.13
* @author: 刘骏帆
************************************************/
uint8 CircleIslandRIn()//入环状态
{
    //这里是判断是否跳出入环状态
    uint8 len=0;
    if (per_l_line_count>EDGELINE_LENGTH) len=EDGELINE_LENGTH;
    else                                  len=per_l_line_count;
    if(f_left_line1[len-1].X-f_left_line1[0].X>JUDGE_IN_EDD_THR && left_line[l_line_count-1].X>USE_IMAGE_W-80)
    {
        return 1;//入环结束
    }
    //对入环状态进行偏差处理
    if(r_line_count<2)//左边90行都丢线，说明只能沿着外边进去
    {
        //重新扫线
        RightLineDetectionAgain('n');
        //对重新扫出来的线的两端来对车体位置进行判断,然后对中线进行处理
        if(right_line[0].X>right_line[r_line_count-1].X)//右边看不到圆环内环，只能看到圆环外环
        {
            uint8 y=0;//
            l_line_count=1;
            for (uint8 i=0;i<r_line_count;i++)
            {
                if(right_line[r_line_count-1-i].Y>=y)
                {
                    y=right_line[r_line_count-1-i].Y;
                }
                else
                {
                    left_line[l_line_count]=right_line[r_line_count-i];
                    l_line_count++;
                }
            }
            per_l_line_count=PER_EDGELINE_LENGTH;
            //对边线进行透视
            EdgeLinePerspective(left_line,l_line_count,per_left_line);
            BlurPoints(per_left_line, l_line_count, f_left_line, LINE_BLUR_KERNEL);
            ResamplePoints(f_left_line, l_line_count, f_left_line1, &per_l_line_count, SAMPLE_DIST*PIXEL_PER_METER);
            track_leftline(f_left_line1, per_l_line_count, center_line_l, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
            track_type=kTrackLeft;
        }
    }
    else
    {
        track_type=kTrackRight;//如果没丢线寻左边线的圆环进去
    }
    return 0;
}

/***********************************************
* @brief : 右环岛检测出环
* @param : 无
* @return: 是否到出环状态 1：环岛出口 0：还在环中
* @date  : 2023.5.13
* @author: 刘骏帆
************************************************/
uint8 CircleIslandROutDetection()//左环岛出环状态
{
    if (l_line_count<10)//左边一开始就丢线
    {
        //左边重新扫线
        LeftLineDetectionAgain('n');
        per_l_line_count=PER_EDGELINE_LENGTH;
        EdgeLinePerspective(left_line,l_line_count,per_left_line);
        BlurPoints(per_left_line, l_line_count, f_left_line, LINE_BLUR_KERNEL);
        ResamplePoints(f_left_line, l_line_count, f_left_line1, &per_l_line_count, SAMPLE_DIST*PIXEL_PER_METER);
        local_angle_points(f_left_line1,per_l_line_count,l_angle,ANGLE_DIST/SAMPLE_DIST);
        nms_angle(l_angle,per_l_line_count,l_angle_1,(ANGLE_DIST/SAMPLE_DIST)*2+1);
        track_leftline(f_left_line1, per_l_line_count, center_line_l, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
    }
    //进行角点判断
    for (int i = 0; i < per_l_line_count; ++i)
    {
        if (70./180.*PI<-l_angle_1[i] && -l_angle_1[i]<140./180.*PI && i<0.8/SAMPLE_DIST)//出环右边角点
        {
            return 1;
        }
    }
    return 0;
}

/***********************************************
* @brief : 右环岛出环
* @param : 无
* @return: 0：出环未结束 1：出环结束
* @date  : 2023.5.13
* @author: 刘骏帆
************************************************/
uint8 CircleIslandROutFinish(void)//检测环岛是否结束
{
    //陀螺仪检测出环
    if(icm_angle_x_flag==1)
    {

        return 1;
    }
    return 0;
}
void CircleIslandROut(void)//环岛出环处理函数
{
    //出环处理
    if (r_line_count>3)//右边如果还能看到线就沿着圆出去
    {
        track_type=kTrackRight;
    }
    else
    {
        if (l_line_count>4)
        {
            //判断角点的位置
            for (int i = 0; i < per_l_line_count; ++i)
            {
                if (70./180.*PI<-l_angle_1[i] && -l_angle_1[i]<140./180.*PI)//出环左边角点
                {
                    if (i>aim_distance/SAMPLE_DIST)
                    {
                        per_l_line_count=i;
                        track_type=kTrackLeft;
                    }
                    else//继承上次的偏差
                    {
                        if (image_bias<-5) image_bias=-8;//如果上次太小就固定打死
                        track_type=kTrackSpecial;
                    }
                    break;
                }
            }
        }
        else//左右边线都丢线
        {
            l_line_count=0;//重新规划左边线
            //找到补线的右上角点
            uint8 half=GRAY_BLOCK/2;
            myPoint right_seed=right_line[r_line_count-1];//右种子
            if (right_seed.Y==0)
            {
                right_seed.Y=USE_IMAGE_H_MAX-half-1,right_seed.X=USE_IMAGE_W-half-2;
            }
            for (; right_seed.Y>USE_IMAGE_H_MIN; right_seed.Y--)
            {
                if(PointSobelTest(right_seed)==1) break;
            }
            myPoint left_seed;//左种子
            left_seed.Y=USE_IMAGE_H-half-1,left_seed.X=half;//图像左下角
            left_line[l_line_count]=left_seed;l_line_count++;//顺序不能错，不然这条线的方向就反了
            left_line[l_line_count]=right_seed;l_line_count++;
            //*****************************
            per_l_line_count=PER_EDGELINE_LENGTH;
            EdgeLinePerspective(left_line,l_line_count,per_left_line);
            ResamplePoints(per_left_line, l_line_count, f_left_line1, &per_l_line_count, SAMPLE_DIST*PIXEL_PER_METER);
            track_leftline(f_left_line1, per_l_line_count, center_line_l, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
            track_type=kTrackLeft;
        }
    }
}

/***********************************************
* @brief : 判断右环岛是否结束
* @param : 无
* @return: 无
* @date  : 2023.5.13
* @author: 刘骏帆
************************************************/
uint8 CircleIslandREnd(void)
{
    if(r_line_count>10)//判断是否走完了环岛
    {
        if(right_line[0].X-right_line[10].X>5)//并且右边线有像左边延生的趋势，利用了梯形畸变近大远小
            return 1;
    }
    //循迹
    track_type=kTrackLeft;//寻左线出去即可
    return 0;
}
/**********************************************************************************************************************/

/***********************************************
* @brief : 从丢线找到不丢线再记录边线，左边线重新扫线
* @param : char choose:选择是否开启重新扫线错边的检测，'y': 开启 'n': 不开启
* @return: 无
* @date  : 2023.5.25
* @author: 刘骏帆
************************************************/
void LeftLineDetectionAgain(char choose)
{
    uint8 half=GRAY_BLOCK/2;
    myPoint left_seed=left_line[l_line_count-1];//左种子
    if (left_seed.Y<USE_IMAGE_H_MIN+half || left_seed.Y>USE_IMAGE_H-half-1 || left_seed.X<half || left_seed.X>USE_IMAGE_W-half-1)
    {
        left_seed.Y=USE_IMAGE_H_MAX-half-1;left_seed.X=half;
    }
    left_seed.X++;
    for (; left_seed.Y > 45; left_seed.Y--)
    {
        if (PointSobelTest(left_seed) == 1)
        {
            break;
        }
    }
    l_line_count=0;//用完之后就重置清除之前扫线的错误数据
    uint8 seed_grown_result=0;//种子生长的结果
    uint8 len=EDGELINE_LENGTH-(USE_IMAGE_H_MAX-left_seed.Y);//重新扫线的长度
    while(l_line_count<len)
    {
        seed_grown_result=EightAreasSeedGrownGray(&left_seed,'l',&left_seed_num);
        if(seed_grown_result==1)
        {
            left_line[l_line_count]=left_seed;l_line_count++;
            l_growth_direction[(left_seed_num+2)%8]++;
        }
        else break;
    }
    if ((l_growth_direction[1]+l_growth_direction[2])<(l_growth_direction[7]+l_growth_direction[6]) && choose=='y')//种子生长的右上生长的趋势比右下要小说明重新扫线是错误的
    {
        l_line_count=0;
    }
}

/***********************************************
* @brief : 从丢线找到不丢线再记录边线，右边线重新扫线
* @param : char choose:选择是否开启重新扫线错边的检测，'y': 开启 'n': 不开启
* @return: 无
* @date  : 2023.5.25
* @author: 刘骏帆
************************************************/
void RightLineDetectionAgain(char choose)
{
    uint8 half=GRAY_BLOCK/2;
    myPoint right_seed=right_line[r_line_count-1];//右种子
    if (right_seed.Y<USE_IMAGE_H_MIN+half || right_seed.Y>USE_IMAGE_H-half-1 || right_seed.X<half || right_seed.X>USE_IMAGE_W-half-1)
    {
        right_seed.Y=USE_IMAGE_H_MAX-half-1,right_seed.X=USE_IMAGE_W-half-1;
    }
    right_seed.X--;
    for (; right_seed.Y > 45; right_seed.Y--)
    {
        if (PointSobelTest(right_seed) == 1)
        {
            break;
        }
    }
    r_line_count=0;//用完之后就重置清除之前扫线的错误数据
    uint8 seed_grown_result=0;//种子生长的结果
    uint8 len=EDGELINE_LENGTH-(USE_IMAGE_H_MAX-right_seed.Y);//重新扫线的长度
    while(r_line_count<len)
    {
        seed_grown_result=EightAreasSeedGrownGray(&right_seed,'r',&right_seed_num);
        if(seed_grown_result==1)
        {
            right_line[r_line_count]=right_seed;r_line_count++;
            r_growth_direction[(right_seed_num+2)%8]++;
        }
        else break;
    }
    if ((r_growth_direction[1]+r_growth_direction[2])<(r_growth_direction[7]+r_growth_direction[6]) && choose=='y')//种子生长的左上生长的趋势比左下要小说明重新扫线是错误的
    {
        r_line_count=0;
    }
}

