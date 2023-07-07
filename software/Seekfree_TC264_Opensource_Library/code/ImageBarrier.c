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

#define BARRIER_COMEBACK_ADC_THR 150 //车回到赛道时的ADC阈值

uint8 barrier_flag=0;

typedef enum BarrierType
{
    kBarrierBegin = 0,
    kBarrierNear,
    kBarrierOut,
    kBarrierIn,
    kBarrierEnd,
}BarrierType;//障碍状态机状态结构体

BarrierType barrier_type = kBarrierBegin;

uint8 BarrierImageVerify(void);//路障图像二次验证

/***********************************************
* @brief : 识别路障的状态机
* @param : void
* @return: 0:路障状态还没结束
*          1:路障状态已经结束
* @date  : 2023.7.5
* @author: 刘骏帆
************************************************/
uint8 BarrierIdentify(void)
{
    switch(barrier_type)
    {
        case kBarrierBegin://检测路障
        {
            dl1a_get_distance();
            if(dl1a_distance_mm <= 1000)//tof检测到路障
            {
                barrier_flag=1;
                //图像二次判断
                if(BarrierImageVerify()==1)
                {
                    gpio_set_level(BEER, 1);
                    speed_type=kNormalSpeed;
                    base_speed=60;
                    StartIntegralAngle_X(40);
                    barrier_type = kBarrierNear;
                }
            }
            break;
        }
        case kBarrierNear://开始陀螺仪积分拐出去
        {
            track_type=kTrackSpecial;//给特定偏差
            image_bias = 5;//舵轮转向是5
            while(!icm_angle_x_flag);
            StartIntegralAngle_X(60);
            barrier_flag=2;
            barrier_type = kBarrierOut;
            break;
        }
        case kBarrierOut://陀螺仪积分拐回来
        {
            track_type=kTrackSpecial;//给特定偏差
            image_bias = -7;//舵轮转向是-5
            while(!icm_angle_x_flag);
            image_bias=0;
            barrier_flag=3;
            barrier_type = kBarrierIn;
            break;
        }
        case kBarrierIn://判断是否拐回赛道
        {
            if (R>BARRIER_COMEBACK_ADC_THR)//电磁有值即回到赛道
            {
                barrier_flag=4;
                barrier_type = kBarrierEnd;
            }
            //没有回到赛道的时候只有右边线没有左边线,那就把右边线给左边线，单边寻左边线
            else if(r_line_count>10 && l_line_count<3)
            {
                memcpy(left_line,right_line,r_line_count);
                l_line_count=r_line_count;
                EdgeLinePerspective(left_line,l_line_count,per_left_line);
                BlurPoints(per_left_line, l_line_count, f_left_line, LINE_BLUR_KERNEL);
                ResamplePoints(f_left_line, l_line_count, f_left_line1, &per_l_line_count, SAMPLE_DIST*PIXEL_PER_METER);
                track_leftline(f_left_line1, per_l_line_count, center_line_l, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
                track_type=kTrackLeft;
            }
            break;
        }
        case kBarrierEnd://判断是否该出状态
        {
            if(r_line_count>10)
            {
                barrier_flag=5;
                gpio_set_level(BEER, 0);
                barrier_type = kBarrierBegin;
                return 1;
            }
            break;
        }
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
        if(temp>(70. / 180. * 3.14) && temp<(120. / 180. * 3.14))
        {
            return 1;
        }
    }
    return 0;
}
