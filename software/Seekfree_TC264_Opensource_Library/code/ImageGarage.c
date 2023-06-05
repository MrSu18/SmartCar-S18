/*
 * ImageGarage.c
 *
 *  Created on: 2023��3��14��
 *      Author: L
 */
#include "zf_common_headfile.h"
#include "ImageTrack.h"
#include "ImageSpecial.h"
#include <math.h>
#include "motor.h"
#include "pid.h"
#include "zf_driver_pit.h"
#include "icm20602.h"
#include "Control.h"

typedef enum GarageType_l
{
    kGarage_In_l = 0,
    kGarage_End_l,
}GarageType_l;//���״̬��״̬�ṹ��

typedef enum GarageType_r
{
    kGarage_In_r = 0,
    kGarage_End_r,
}GarageType_r;//�ҿ�״̬��״̬�ṹ��

GarageType_l garage_type_l = kGarage_In_l;
GarageType_r garage_type_r = kGarage_In_r;
/***********************************************
* @brief : ���״̬��
* @param : void
* @return: 0:����ʮ��·
*          1:ʶ�𵽶�·
* @date  : 2023.5.5
* @author: L
************************************************/
uint8 GarageIdentify_L(void)
{
    switch (garage_type_l)
    {
        case kGarage_In_l://�ж��Ƿ������½ǵ㣬���ұ��ߴ��ڣ�Ѱ����
        {
            int corner_id = 0;
            if (GarageFindCorner(&corner_id) == 1)//�ж��Ƿ��ҵ��ǵ�
            {
                StartIntegralAngle_X(70);//���������ǽǶȻ���
                speed_type=kNormalSpeed;//����
                base_speed = 60;
                track_type = kTrackRight;//Ѱ����
                if (corner_id < 18)//�ǵ�ܽ�ʱ�л���һ��״̬
                    garage_type_l = kGarage_End_l;
            }
            break;
        }
        case kGarage_End_l://���̶�ƫ������ǻ���һ���ǶȺ�ͣ��
        {
            image_bias = 10;
            while(!icm_angle_x_flag);

            while(1)
            {
                base_speed = 0;
                target_speed = 0;
                target_right = 0;
                image_bias = 0;
                system_delay_ms(200);
                pit_disable(CCU60_CH0);
                pit_disable(CCU60_CH1);
                MotorSetPWM(0,0);
            }
            break;
        }
        default:break;
    }
    return 0;
}
/***********************************************
* @brief : �ҿ�״̬��
* @param : void
* @return: 0:����ʮ��·
*          1:ʶ�𵽶�·
* @date  : 2023.5.9
* @author: L
************************************************/
uint8 GarageIdentify_R(void)
{
    switch (garage_type_r)
    {
        case kGarage_In_r://�ж��Ƿ������½ǵ㣬������ߴ��ڣ�Ѱ����
        {
            int corner_id = 0;
            if (GarageFindCorner(&corner_id) == 2)//�ж��Ƿ��ҵ��ǵ�
            {
                StartIntegralAngle_X(70);//���������ǽǶȻ���
                speed_type=kNormalSpeed;//����
                base_speed = 60;
                track_type = kTrackLeft;//Ѱ����
                if (corner_id < 18)//�ǵ�ܽ�ʱ�л���һ��״̬
                    garage_type_r = kGarage_End_r;
            }
            break;
        }
        case kGarage_End_r://���̶�ƫ������ǻ���һ���ǶȺ�ͣ��
        {
            image_bias = -10;
            while(!icm_angle_x_flag);
            turnpid_image.last_err = 0;

            while(1)
            {
                base_speed = 0;
                target_speed = 0;
                target_right = 0;
                image_bias = 0;
                system_delay_ms(200);
                pit_disable(CCU60_CH0);
                pit_disable(CCU60_CH1);
                MotorSetPWM(0,0);
            }
            break;
        }
        default:break;
    }
    return 0;
}
/***********************************************
* @brief : �����ҽǵ�
* @param : void
* @return: 0:����ʮ��·
*          1:ʶ�𵽶�·
* @date  : 2023.5.9
* @author: L
************************************************/
uint8 GarageFindCorner(int* corner_id)
{
    bool garage_find_l = false, garage_find_r = false;

    for (int i = 0; i < l_line_count; i++)
    {
        if (l_angle_1[i] == 0) continue;
        if (garage_find_l == false && fabs(l_angle_1[i]) > 70 * 3.14 / 180 && fabs(l_angle_1[i]) < 120 * 3.14 / 180)
        {
            garage_find_l = true;
            *corner_id = i;
        }
    }
    for (int i = 0; i < r_line_count; i++)
    {
        if (r_angle_1[i] == 0) continue;
        if (garage_find_r == false && (fabs(r_angle_1[i]) > 70 * 3.14 / 180) && (fabs(r_angle_1[i]) < 120 * 3.14 / 180))
        {
            garage_find_r = true;
            *corner_id = i;
        }
    }

    if (garage_find_l == true && garage_find_r == false && r_line_count > 40)
        return 1;
    else if (garage_find_r == true && garage_find_l == false && l_line_count > 40)
        return 2;
    else
        return 0;
}
