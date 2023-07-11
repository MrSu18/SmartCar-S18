/*
 * ImageBarrier.c
 *
 *  Created on: 2023年4月15日
 *      Author: L
 */
#include "ImageSpecial.h"
#include "zf_device_dl1a.h"
#include "motor.h"
#include "ImageTrack.h"
#include "icm20602.h"
#include "Control.h"
#include "string.h"
#include "debug.h"
#include "zf_driver_gpio.h"
#include "adc.h"
#include "math.h"
#include "ImageProcess.h"

#define TOF_DISTANCE_THR 2000//TOF测距的阈值
#define IMAGE_DISTANCE_THR  0.65//图像拐点的阈值
#define ADC_IN_TRACK_THR    300//判断车是否在赛道内的ADC阈值

uint8 barrier_status=0;//路障状态
uint8 barrier_icm_flag=0;//路障特殊陀螺仪标志变量

uint8 BarrierImageVerify(void);//路障图像二次验证

/***********************************************
* @brief : 识别路障的状态机
* @param : void
* @return: 0:路障状态还没结束
*          1:路障状态已经结束
* @date  : 2023.7.10
* @author: 刘骏帆
************************************************/
uint8 BarrierIdentify(void)
{
    speed_type=kNormalSpeed;
    switch(barrier_status)
    {
        case 0://识别路障
            if(BarrierImageVerify()==1)//识别到拐点就是路障
            {
                //tof二次判断
                dl1a_get_distance();
                if(dl1a_distance_mm <= TOF_DISTANCE_THR && dl1a_finsh_flag==1)//tof检测到路障
                {
                    barrier_status=1;
                    gpio_set_level(BEER, 1);//识别到开蜂鸣器
                    speed_type=kNormalSpeed;base_speed=process_property[process_status_cnt].min_speed;//降速
                    track_type=kTrackSpecial;image_bias=6;//左拐
                    encoder_dis_flag=1;
                    StartIntegralAngle_X(40);while(!icm_angle_x_flag);//等待40度
                    image_bias=0;
                    while(dis<575.11);encoder_dis_flag=0;
                    track_type=kTrackSpecial;image_bias=-8;//右拐
                    encoder_dis_flag=1;
                    StartIntegralAngle_X(90);while(!icm_angle_x_flag);//等待70度
                    image_bias=0;
                    while(dis<780);encoder_dis_flag=0;
//                    return 1;
                }
            }
            break;
        case 1://写死拐出去拐回来后，判断此时的左边线是不是真的左边线
            //左边有边线，并且是往左上生长的，说明是正常的回到了赛道，结束状态
            if(l_line_count>20 && r_line_count<3 && (l_growth_direction[4]+l_growth_direction[3]-l_growth_direction[1]-l_growth_direction[0])>l_line_count-20)
            {
                gpio_set_level(BEER, 0);//状态结束关闭蜂鸣器
                speed_type=kImageSpeed;
                encoder_dis_flag=0;
                barrier_status=0;
                return 1;
            }
            //电磁判断是否在赛道内
            else if(L<ADC_IN_TRACK_THR && LM<ADC_IN_TRACK_THR && M<ADC_IN_TRACK_THR && RM<ADC_IN_TRACK_THR && R<ADC_IN_TRACK_THR)
            {
                //验证右边线是否是赛道要走的左边
                if(r_line_count>20 && (r_growth_direction[1]+r_growth_direction[0]-r_growth_direction[4]-r_growth_direction[3])>r_line_count-20)
                {
                    track_type=kTrackSpecial;//在这里面求偏差
                    memcpy(left_line,right_line,2*r_line_count);//结构体是两个字节所以*2
                    l_line_count=r_line_count;
                    EdgeLinePerspective(left_line,l_line_count,per_left_line);
                    BlurPoints(per_left_line, l_line_count, f_left_line, LINE_BLUR_KERNEL);
                    per_l_line_count=PER_EDGELINE_LENGTH;//插值之前要先重置这个
                    ResamplePoints(f_left_line, l_line_count, f_left_line1, &per_l_line_count, SAMPLE_DIST*PIXEL_PER_METER);
                    track_leftline(f_left_line1, per_l_line_count, center_line_l, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(0.25));
                    image_bias = GetAnchorPointBias(aim_distance, per_l_line_count, center_line_l);
                }
            }
            break;
        default:break;
    }
    return 0;
}

/***********************************************
* @brief : 使用角点二次验证是否是路障
* @param : void
* @return: 0:不是路障
*          1:识别到路障
* @date  : 2023.7.5
* @author: 刘骏帆
************************************************/
uint8 BarrierImageVerify(void)
{
    uint8 len=0;
    float* angle;
    //哪条边线比较长就去遍历哪条边线
    if(l_line_count>r_line_count)
    {
        len=l_line_count;
        angle=l_angle_1;
    }
    else
    {
        len=r_line_count;
        angle=r_angle_1;
    }
    //遍历边线查看是否有角点
    for(uint8 i=0;i<len;i++)
    {
        float temp=fabs(angle[i]);
        if(temp>(70. / 180. * 3.14) && temp<(120. / 180. * 3.14) && i<IMAGE_DISTANCE_THR/SAMPLE_DIST)
        {
            return 1;
        }
    }
    return 0;
}
