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
}GarageType_l;

typedef enum GarageType_r
{
    kGarage_Begin_r = 0,
    kGarage_In_r,
    kGarage_End_r,
}GarageType_r;

GarageType_l garage_type_l = kGarage_In_l;
GarageType_r garage_type_r = kGarage_Begin_r;
uint8 flag = 0;
int temp = 0;

uint8 GarageIdentify_L(void)
{
    switch (garage_type_l)
    {
    case kGarage_In_l://判断是否有左下角点，且右边线存在，寻左线，切换到kGarare_In_l
    {
        int corner_id = 0;
        if (GarageFindCorner(&corner_id) == 1)//判断角点还有电磁偏差是否为0左右
        {
            StartIntegralAngle_X(70);
            gpio_toggle_level(P21_5);
            speed_type=kNormalSpeed;
            base_speed = 50;
            track_type = kTrackRight;
            if (corner_id < 18)
            {
                gpio_toggle_level(P21_3);
                garage_type_l = kGarage_End_l;
            }
        }
        break;
    }
    case kGarage_End_l:
    {
        gpio_toggle_level(P20_9);
        image_bias = 10;
        while(!icm_angle_x_flag);
        turnpid_image.last_err = 0;
//        system_delay_ms(400);

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

uint8 GarageIdentify_R(void)
{
    int corner_id = 0;
    switch (garage_type_r)
    {
    case kGarage_Begin_r://判断是否有左下角点，且右边线存在，寻左线，切换到kGarare_In_l
    {
        if (GarageFindCorner(&corner_id) == 1)//判断角点还有电磁偏差是否为0左右
        {
            track_type = kTrackLeft;
            if (corner_id < 4)
                garage_type_r = kGarage_In_r;
        }
        break;
    }
    case kGarage_In_r:
    {
//        EdgeDetection_Garage('r');
//
//        BlurPoints(right_line, r_line_count, f_right_line, LINE_BLUR_KERNEL);
//        local_angle_points(f_right_line, r_line_count, r_angle, ANGLE_DIST / SAMPLE_DIST);
//        nms_angle(r_angle, r_line_count, r_angle_1, (ANGLE_DIST / SAMPLE_DIST) * 2 + 1);
//
//        //uint8 temp = GarageFindCorner(&corner_id);
//        if (GarageFindCorner(&corner_id) == 1)
//        {
//            r_line_count = corner_id;
//            track_type = kTrackRight;
//        }
        if (l_line_count < 2)
            garage_type_r = kGarage_End_r;
        break;
    }
    case kGarage_End_r://出现两个角点，停车
    {
        track_type = kTrackLeft;
        if (GarageFindCorner(&corner_id) == 3)
        {
          base_speed = 0;
          pit_disable(CCU60_CH0);
            return 1;
        }
        break;
    }
    default:break;
    }
    return 0;
}

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
    else if (garage_find_l == true && garage_find_r == true)
        return 3;
    else
        return 0;
}

void EdgeDetection_Garage(uint8 flag)
{
    switch (flag)
    {
    case 'l':
    {
        //进行重新扫线
        myPoint left_seed = left_line[l_line_count - 1];//左种子
//        myPoint last_left_seed = left_seed;
        l_line_count = 0;//用完之后就重置清除之前扫线的错误数据
        uint8 left_seed_num = 0;//左种子八零域标号
//        uint8 last_seed_num_l = 0;
        uint8 seed_grown_result = 0;//种子生长的结果
        uint8 flag = 0;//从丢线到不丢线,0:还没找到过边界，1:已经找到边界
//        uint8 zebra_flag = 0;
        while (l_line_count < EDGELINE_LENGTH - 1)
        {
            seed_grown_result = EightAreasSeedGrown(&left_seed, 'l', &left_seed_num);
            if (seed_grown_result == 1)
            {
//                if (zebra_flag == 0)
//                {
//                    int black_count = 1;
//                    int black_high = 0;
//                    for (int i = 0; i < 20; i++)
//                    {
//                        if (use_image[USE_IMAGE_H - left_seed.Y][i] == IMAGE_BLACK)
//                            black_count++;
//                        if (black_count >= 4)
//                        {
//                            black_high++;
//                            left_seed = last_left_seed;
//                            left_seed_num = last_seed_num_l;
//                            l_line_count = 0;
//                            break;
//                        }
//                        if (black_high >= 8)
//                            zebra_flag = 1;
//                    }
//                    last_left_seed = left_seed;
//                    last_seed_num_l = left_seed_num;
//                }
//                else
//                {
                    flag = 1;
                    left_line[l_line_count] = left_seed; l_line_count++;
//                }
            }
            else if (seed_grown_result == 2)
            {
                if (flag == 0) continue;
                else        break;
            }
            else break;
        }
        break;
    }
    case 'r':
    {
        //进行重新扫线
        myPoint right_seed = right_line[r_line_count - 1];//左种子
        myPoint last_right_seed = right_seed;
        r_line_count = 0;//用完之后就重置清除之前扫线的错误数据
        uint8 right_seed_num = 0;//左种子八零域标号
        uint8 last_seed_num_r = 0;
        uint8 seed_grown_result = 0;//种子生长的结果
        uint8 flag = 0;//从丢线到不丢线,0:还没找到过边界，1:已经找到边界
        uint8 zebra_flag = 0;
        while (r_line_count < EDGELINE_LENGTH)
        {
            seed_grown_result = EightAreasSeedGrown(&right_seed, 'r', &right_seed_num);
            if (seed_grown_result == 1)
            {
                if (zebra_flag == 0)
                {
                    int black_count = 1;
                    int black_high = 0;
                    for (int i = 0; i < 20; i++)
                    {
                        if (use_image[USE_IMAGE_H - right_seed.Y][i] == IMAGE_BLACK)
                            black_count++;
                        if (black_count >= 4)
                        {
                            black_high++;
                            right_seed = last_right_seed;
                            right_seed_num = last_seed_num_r;
                            r_line_count = 0;
                            break;
                        }
                        if (black_high >= 8)
                            zebra_flag = 1;
                    }
                    last_right_seed = right_seed;
                    last_seed_num_r = right_seed_num;
                }
                else
                {
                    flag = 1;
                    right_line[r_line_count] = right_seed; r_line_count++;
                }
            }
            else if (seed_grown_result == 2)
            {
                if (flag == 0) continue;
                else        break;
            }
            else break;
        }
        break;
    }
    default: break;
    }
}
