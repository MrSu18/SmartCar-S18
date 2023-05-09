/*
 * ImageBarrier.c
 *
 *  Created on: 2023��4��15��
 *      Author: L
 */
#include "ImageSpecial.h"
#include "zf_device_dl1a.h"
#include "motor.h"
#include "ImageTrack.h"

typedef enum BarrierType
{
    kBarrierBegin = 0,
    kBarrierNear,
    kBarrierEnd,
}BarrierType;//�ϰ�״̬��״̬�ṹ��

BarrierType barrier_type = kBarrierBegin;
/***********************************************
* @brief : ʶ��·�ϵ�״̬��
* @param : void
* @return: 0:����·��
*          1:ʶ��·��
* @date  : 2023.4.15
* @author: L
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
                base_speed = 140;
                barrier_type = kBarrierNear;
            }
            else base_speed = 160;
            break;
        }
        case kBarrierNear:
        {
            image_bias = 18 - time;
            timeintegral_flag = 1;
            system_delay_ms(360);
            barrier_type = kBarrierEnd;
            break;
        }
        case kBarrierEnd:
        {
            image_bias = 18;
            system_delay_ms(200);
            barrier_type = kBarrierBegin;
            return 1;
            break;
        }
        default:break;
    }
    return 0;
}

