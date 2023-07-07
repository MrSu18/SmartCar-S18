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
#include "math.h"

#define BARRIER_COMEBACK_ADC_THR 150 //���ص�����ʱ��ADC��ֵ

uint8 barrier_flag=0;

typedef enum BarrierType
{
    kBarrierBegin = 0,
    kBarrierNear,
    kBarrierOut,
    kBarrierIn,
    kBarrierEnd,
}BarrierType;//�ϰ�״̬��״̬�ṹ��

BarrierType barrier_type = kBarrierBegin;

uint8 BarrierImageVerify(void);//·��ͼ�������֤

/***********************************************
* @brief : ʶ��·�ϵ�״̬��
* @param : void
* @return: 0:·��״̬��û����
*          1:·��״̬�Ѿ�����
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
            if(dl1a_distance_mm <= 1000)//tof��⵽·��
            {
                barrier_flag=1;
                //ͼ������ж�
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
        case kBarrierNear://��ʼ�����ǻ��ֹճ�ȥ
        {
            track_type=kTrackSpecial;//���ض�ƫ��
            image_bias = 5;//����ת����5
            while(!icm_angle_x_flag);
            StartIntegralAngle_X(60);
            barrier_flag=2;
            barrier_type = kBarrierOut;
            break;
        }
        case kBarrierOut://�����ǻ��ֹջ���
        {
            track_type=kTrackSpecial;//���ض�ƫ��
            image_bias = -7;//����ת����-5
            while(!icm_angle_x_flag);
            image_bias=0;
            barrier_flag=3;
            barrier_type = kBarrierIn;
            break;
        }
        case kBarrierIn://�ж��Ƿ�ջ�����
        {
            if (R>BARRIER_COMEBACK_ADC_THR)//�����ֵ���ص�����
            {
                barrier_flag=4;
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
* @brief : ʹ�ýǵ������֤�Ƿ���·��
* @param : void
* @return: 0:����·��
*          1:ʶ��·��
* @date  : 2023.7.5
* @author: ������
************************************************/
uint8 BarrierImageVerify(void)
{
    uint8 len=0;
    float* angle;
    //�������߱Ƚϳ���ȥ������������
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
    //�������߲鿴�Ƿ��нǵ�
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
