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
#include "ImageProcess.h"

/***********************************************
* @brief : �µ�״̬��
* @param : void
* @return: 0:�����µ�
*          1:ʶ���µ�
* @date  : 2023.7.5
* @author: ������
************************************************/
uint8 SlopeIdentify(void)
{
    static uint8 status=0;//�µ�״̬ת�Ʊ���
    tft180_show_uint(120, 100, status, 2);
    switch(status)
    {
        case 0://���������Ǽ������
            StartIntegralAngle_Y(30);//�������
            status=1;
            break;
        case 1://ʶ���µ�
            //tof����µ�
            dl1a_get_distance();
            //ͼ�����µ�
            float gradient_l = 0;
            float gradient_r = 0;
            gradient_l = CalculateGradient('l');//������б��
            gradient_r = CalculateGradient('r');//������б��
            //���������ͼ���tof���Լ�⵽�µ�
            if(((gradient_l * gradient_r < 0) && gradient_l < 0.5 && gradient_l > 0.2 && gradient_r < -0.2 && gradient_r > -0.5)||(dl1a_distance_mm < 600)||(icm_angle_y_flag==1))
            {
                gpio_set_level(BEER,1);//����������
                speed_type=kNormalSpeed;//�ر��ٶȾ���
                base_speed = process_property[process_status_cnt].min_speed;//����
                track_mode = kTrackADC;//�л��ɵ��ѭ��
                StartIntegralAngle_Y(-30);//�������
                status=2;//��ת״̬���������״̬
            }
            break;
        case 2://�������
            if(icm_angle_y_flag==1)
            {
                encoder_dis_flag = 1;//�������������֣������ѭ��
                base_speed=original_speed;//�ָ��ٶȣ�����ѭ�����ǵ��
                status=3;
            }
            break;
        case 3://�µ�����
            if(dis>500)//50cm������
            {
                track_mode=kTrackImage;//�л���ͼ��
                base_speed=original_speed;//�ָ��ٶ�
                speed_type=kImageSpeed;//�����ٶȾ���
                gpio_set_level(BEER,0);
                status=0;
                return 1;
            }
            break;
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
