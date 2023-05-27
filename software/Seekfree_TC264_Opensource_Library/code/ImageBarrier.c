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

typedef enum BarrierType
{
    kBarrierBegin = 0,
    kBarrierNear,
    kBarrierOut,
    kBarrierEnd,
}BarrierType;//�ϰ�״̬��״̬�ṹ��

BarrierType barrier_type = kBarrierBegin;
/***********************************************
* @brief : ʶ��·�ϵ�״̬��
* @param : void
* @return: 0:����·��
*          1:ʶ��·��
* @date  : 2023.5.19
* @author: L & ������
************************************************/
uint8 BarrierIdentify(void)
{
    switch(barrier_type)
    {
        case kBarrierBegin://tof��⵽·��
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
        case kBarrierNear://��ʼ�����ǻ��ֹճ�ȥ
        {
            image_bias = 6;//����ת����5
            while(!icm_angle_x_flag);
            image_bias=0;
            StartIntegralAngle_X(70);
            barrier_type = kBarrierOut;
            break;
        }
        case kBarrierOut://�����ǻ��ֹջ���
        {
            image_bias = -6;//����ת����-5
            while(!icm_angle_x_flag);
            image_bias=0;
            barrier_type = kBarrierEnd;
            break;
        }
        case kBarrierEnd://ͼ������Ƿ�ص�����
        {
            if (l_line_count>10)//������ߵ�ʱ�򼴻ص�����
            {
                barrier_type = kBarrierBegin;
                return 1;
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
        default:break;
    }
    return 0;
}

