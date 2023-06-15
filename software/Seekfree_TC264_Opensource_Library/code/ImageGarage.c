/*
 * ImageGarage.c
 *
 *  Created on: 2023年3月14日
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
}GarageType_l;//左库状态机状态结构体

typedef enum GarageType_r
{
    kGarage_In_r = 0,
    kGarage_End_r,
}GarageType_r;//右库状态机状态结构体

typedef enum GarageType
{
    kGarage_In = 0,
    kGarage_End,
}GarageType;

GarageType_l garage_type_l = kGarage_In_l;
GarageType_r garage_type_r = kGarage_In_r;
GarageType   garage_type = kGarage_In;

/***********************************************
* @brief : 左库状态机
* @param : void
* @return: 0:不是十断路
*          1:识别到断路
* @date  : 2023.5.5
* @author: L
************************************************/
uint8 GarageIdentify_L(void)
{
    switch (garage_type_l)
    {
        case kGarage_In_l://判断是否有左下角点，且右边线存在，寻右线
        {
            int corner_id = 0;
            if (GarageFindCorner(&corner_id) == 1)//判断是否找到角点
            {
                StartIntegralAngle_X(70);//开启陀螺仪角度积分
                speed_type=kNormalSpeed;//降速
                base_speed = 65;
                track_type = kTrackRight;//寻右线
                if (corner_id < 18)//角点很近时切换下一个状态
                    garage_type_l = kGarage_End_l;
            }
            break;
        }
        case kGarage_End_l://给固定偏差，陀螺仪积分一定角度后停下
        {
            image_bias = 10;
            while(!icm_angle_x_flag);

            while(1)
            {
                base_speed = 0;
                target_left = 0;
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
* @brief : 右库状态机
* @param : void
* @return: 0:不是十断路
*          1:识别到断路
* @date  : 2023.5.9
* @author: L
************************************************/
uint8 GarageIdentify_R(void)
{
    switch (garage_type_r)
    {
        case kGarage_In_r://判断是否有右下角点，且左边线存在，寻左线
        {
            int corner_id = 0;
            if (GarageFindCorner(&corner_id) == 2)//判断是否找到角点
            {
                StartIntegralAngle_X(70);//开启陀螺仪角度积分
                speed_type=kNormalSpeed;//降速
                base_speed = 60;
                track_type = kTrackLeft;//寻左线
                if (corner_id < 18)//角点很近时切换下一个状态
                    garage_type_r = kGarage_End_r;
            }
            break;
        }
        case kGarage_End_r://给固定偏差，陀螺仪积分一定角度后停下
        {
            image_bias = -10;
            while(!icm_angle_x_flag);
            turnpid_image.last_err = 0;

            while(1)
            {
                base_speed = 0;
                target_left = 0;
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
* @brief : 右库状态机
* @param : void
* @return: 0:不是十断路
*          1:识别到断路
* @date  : 2023.5.9
* @author: L
************************************************/
uint8 GarageIdentify_Straight(void)
{
    uint8 lr_flag = 0;//左库或右库的标志位
    switch(garage_type)
    {
        case kGarage_In://判断是否有角点且另外一边的边线存在
        {
            int corner_id = 0;
            if(GarageFindCorner(&corner_id) == 1)
            {
                track_type = kTrackRight;//寻右线
                lr_flag = 1;//是左车库
            }
            else if(GarageFindCorner(&corner_id) == 2)
            {
                track_type = kTrackLeft;//寻左线
                lr_flag = 2;//是右车库
            }

            if(corner_id < 5 && lr_flag != 0)
            {
                garage_type = kGarage_End;//切换下一个状态
                encoder_dis_flag = 1;//开启编码器测距
            }
            break;
        }
        case kGarage_End://编码器测距出状态
        {
            if(lr_flag == 1)
                track_type = kTrackRight;//默认寻右线
            else if(lr_flag == 2)
                track_type = kTrackLeft;//默认寻左线

            if(dis > 450)//积分45cm出状态
            {
                garage_type = kGarage_In;//复位状态机
                encoder_dis_flag = 0;//关闭编码器测距
                return 1;
            }
            break;
        }
    }
    return 0;
}
/***********************************************
* @brief : 车库找角点
* @param : void
* @return: 0:不是十断路
*          1:识别到断路
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
