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
#include "icm20602.h"
#include "Control.h"
#include "string.h"
#include "debug.h"
#include "zf_driver_gpio.h"
#include "adc.h"

#define BARRIER_COMEBACK_ADC_THR 1000 //���ص�����ʱ��ADC��ֵ

typedef enum BarrierType
{
    kBarrierBegin = 0,
    kBarrierNear,
    kBarrierOut,
    kBarrierIn,
    kBarrierEnd,
}BarrierType;//�ϰ�״̬��״̬�ṹ��

BarrierType barrier_type = kBarrierBegin;
/***********************************************
* @brief : ʶ��·�ϵ�״̬��
* @param : void
* @return: 0:����·��
*          1:ʶ��·��
* @date  : 2023.7.5
* @author: ������
************************************************/
uint8 BarrierIdentify(void)
{
    switch(barrier_type)
    {
        case kBarrierBegin://���·��
        {
            dl1a_get_distance();
            if(dl1a_distance_mm <= 900)//tof��⵽·��
            {
                //ͼ������ж�
//                if(left_line[l_line_count-1].X>right_line[r_line_count-1].X)
//                {
                    gpio_set_level(BEER, 1);
                    speed_type=kNormalSpeed;
                    base_speed=60;
                    StartIntegralAngle_X(40);
                    barrier_type = kBarrierNear;
//                }
            }
            break;
        }
        case kBarrierNear://��ʼ�����ǻ��ֹճ�ȥ
        {
            track_type=kTrackSpecial;//���ض�ƫ��
            image_bias = 5;//����ת����5
            while(!icm_angle_x_flag);
            StartIntegralAngle_X(70);
            barrier_type = kBarrierOut;
            break;
        }
        case kBarrierOut://�����ǻ��ֹջ���
        {
            track_type=kTrackSpecial;//���ض�ƫ��
            image_bias = -7;//����ת����-5
            while(!icm_angle_x_flag);
            barrier_type = kBarrierIn;
            break;
        }
        case kBarrierIn://�ж��Ƿ�ջ�����
        {
            if (l_line_count>10)//�����ֵ���ص�����
            {
                barrier_type = kBarrierEnd;
            }
            //û�лص�������ʱ��ֻ���ұ���û�������,�ǾͰ��ұ��߸�����ߣ�����Ѱ�����
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
        case kBarrierEnd://�ж��Ƿ�ó�״̬
        {
            if(r_line_count>10)
            {
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

