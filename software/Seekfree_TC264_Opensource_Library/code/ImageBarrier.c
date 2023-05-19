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

typedef enum BarrierType
{
    kBarrierBegin = 0,
    kBarrierNear,
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
int time = 0;
uint8 timeintegral_flag = 0;
uint8 BarrierIdentify(void)
{
    switch(barrier_type)
    {
        case kBarrierBegin:
        {
            dl1a_get_distance();
            if(dl1a_distance_mm <= 1000)
            {
                speed_type=kNormalSpeed;
//                base_speed = 50;
                StartIntegralAngle_X(50);
                barrier_type = kBarrierNear;
            }
//            else base_speed = 65;
            break;
        }
        case kBarrierNear:
        {
            image_bias = 5;
            while(!icm_angle_x_flag);
            image_bias=0;
//            encoder_dis_flag = 1;
//            while(dis < 300);
//            encoder_dis_flag = 0;
            StartIntegralAngle_X(80);
            barrier_type = kBarrierEnd;
            break;
        }
        case kBarrierEnd:
        {
            image_bias = -5;
            while(!icm_angle_x_flag);
            image_bias=0;
//            encoder_dis_flag = 1;
//            while(dis < 300);
//            encoder_dis_flag = 0;
            barrier_type = kBarrierBegin;
            return 1;
            break;
        }
        default:break;
    }
    return 0;
}

