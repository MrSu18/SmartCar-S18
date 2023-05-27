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

typedef enum BarrierType
{
    kBarrierBegin = 0,
    kBarrierNear,
    kBarrierOut,
    kBarrierEnd,
}BarrierType;//障碍状态机状态结构体

BarrierType barrier_type = kBarrierBegin;
/***********************************************
* @brief : 识别路障的状态机
* @param : void
* @return: 0:不是路障
*          1:识别到路障
* @date  : 2023.5.19
* @author: L & 刘骏帆
************************************************/
uint8 BarrierIdentify(void)
{
    switch(barrier_type)
    {
        case kBarrierBegin://tof检测到路障
        {
            dl1a_get_distance();
            if(dl1a_distance_mm <= 1000)
            {
                speed_type=kNormalSpeed;
                StartIntegralAngle_X(50);
                barrier_type = kBarrierNear;
            }
            break;
        }
        case kBarrierNear://开始陀螺仪积分拐出去
        {
            image_bias = 6;//舵轮转向是5
            while(!icm_angle_x_flag);
            image_bias=0;
            StartIntegralAngle_X(70);
            barrier_type = kBarrierOut;
            break;
        }
        case kBarrierOut://陀螺仪积分拐回来
        {
            image_bias = -6;//舵轮转向是-5
            while(!icm_angle_x_flag);
            image_bias=0;
            barrier_type = kBarrierEnd;
            break;
        }
        case kBarrierEnd://图像控制是否回到赛道
        {
            if (l_line_count>10)//有左边线的时候即回到赛道
            {
                barrier_type = kBarrierBegin;
                return 1;
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
        default:break;
    }
    return 0;
}

