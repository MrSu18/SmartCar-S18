/*
 * ImageSlope.c
 *
 *  Created on: 2023��4��16��
 *      Author: L
 */
#include "zf_common_headfile.h"
#include "ImageTrack.h"
#include "motor.h"
#include "ImageSpecial.h"
#include "Control.h"
#include "stdlib.h"
#include "icm20602.h"
#include "adc.h"
#include "debug.h"

typedef enum SlopeType
{
    kSlopeBegin = 0,
    kSlopeUp,
    kSlopeEnd,
}SlopeType;//�µ�״̬��״̬�ṹ��

SlopeType slope_type = kSlopeBegin;

/***********************************************
* @brief : �µ�״̬��
* @param : void
* @return: 0:�����µ�
*          1:ʶ���µ�
* @date  : 2023.4.21
* @author: L
************************************************/
uint8 SlopeIdentify(void)
{
    switch(slope_type)
    {
        case kSlopeBegin://�������������ߵ�б�ʣ�б���෴��б����0.2��0.5֮��
        {
            dl1a_get_distance();//���
            float gradient_l = 0;
            float gradient_r = 0;
            gradient_l = CalculateGradient('l');//������б��
            gradient_r = CalculateGradient('r');//������б��
            if(((gradient_l * gradient_r < 0) && gradient_l < 0.5 && gradient_l > 0.2 && gradient_r < -0.2 && gradient_r > -0.5)||(dl1a_distance_mm < 600))
            {
                gpio_set_level(BEER,1);//����������
                speed_type=kNormalSpeed;//�ر��ٶȾ���
                base_speed = 58;//����
                last_track_mode = track_mode;//�л��ɵ��ѭ��
                track_mode = kTrackADC;
                slope_type = kSlopeUp;//�л���һ��״̬
                encoder_dis_flag = 1;//��������������
            }
            break;
        }
        case kSlopeUp:
        {
            dl1a_get_distance();
            if(dl1a_distance_mm > 650)
                slope_type = kSlopeEnd;
            break;
        }
        case kSlopeEnd:
        {
            if((dis >= 2500) || (NORMAL_M >= 4095))//����������1.2m�������µ�״̬
            {
                encoder_dis_flag = 0;//���������ֱ�־λ����
                base_speed = original_speed;
                last_track_mode = track_mode;//�ĳ�ͼ��ѭ��
                speed_type=kImageSpeed;//�����ٶȾ���
                track_mode = kTrackImage;
                slope_type = kSlopeBegin;//��λ״̬��
                gpio_set_level(BEER,0);//�رշ�����
                return 1;
            }
            break;
        }
        default:break;
    }
    return 0;
}
/***********************************************
* @brief : �µ��������б��
* @param : flag:ѡ�����߻�������
* @return: gradient:����б��
* @date  : 2023.4.21
* @author: L
************************************************/
float CalculateGradient(uint8 lr_flag)
{
    switch(lr_flag)
    {
        case 'l'://������б��
        {
            if(per_l_line_count < 160) return 0;//���ߺܳ�������0
            float x = 0,y = 0,xx = 0,xy = 0;
            float x_ave = 0,y_ave = 0,xx_ave = 0,xy_ave = 0;
            float gradient = 0;
            for(int i = 0;i < 160;i++)
            {
                x += f_left_line1[i].Y;
                y += f_left_line1[i].X;
                xx += f_left_line1[i].Y * f_left_line1[i].Y;
                xy += f_left_line1[i].Y * f_left_line1[i].X;
            }
            x_ave = x / 160.0;
            y_ave = y / 160.0;
            xx_ave = xx / 160.0;
            xy_ave = xy / 160.0;

            gradient = (xy_ave - x_ave * y_ave)/(xx_ave - x_ave * x_ave);
            return gradient;
            break;
        }
        case 'r'://������б��
        {
            if(per_r_line_count < 160) return 0;//���ߺܳ�������0
            float x = 0,y = 0,xx = 0,xy = 0;
            float x_ave = 0,y_ave = 0,xx_ave = 0,xy_ave = 0;
            float gradient = 0;
            for(int i = 0;i < 160;i++)
            {
                x += f_right_line1[i].Y;
                y += f_right_line1[i].X;
                xx += f_right_line1[i].Y * f_right_line1[i].Y;
                xy += f_right_line1[i].Y * f_right_line1[i].X;
            }
            x_ave = x / 160.0;
            y_ave = y / 160.0;
            xx_ave = xx / 160.0;
            xy_ave = xy / 160.0;

            gradient = (xy_ave - x_ave * y_ave)/(xx_ave - x_ave * x_ave);
            return gradient;
            break;
        }
        default:break;
    }
    return 0;
}
