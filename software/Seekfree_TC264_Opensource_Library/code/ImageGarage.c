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
#include "zf_driver_pit.h"

typedef enum GarageType_l
{
    kGarage_In_l = 0,
    kGarage_End_l,
}GarageType_l;

typedef enum GarageType_r
{
    kGarage_In_r = 0,
    kGarage_End_r,
}GarageType_r;

GarageType_l garage_type_l = kGarage_In_l;
GarageType_r garage_type_r = kGarage_In_r;

uint8 GarageIdentify_L(void)
{
    switch (garage_type_l)
    {
    case kGarage_In_l://判断是否有左下角点，且右边线存在，寻左线，切换到kGarare_In_l
    {
        if (GarageFindCorner() == 1)
            track_type = kTrackLeft;

        break;
    }
    case kGarage_End_l://出现两个角点，停车
    {
        if (GarageFindCorner() == 3)
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

uint8 GarageIdentify_R(void)
{
    switch (garage_type_r)
    {
    case kGarage_In_r://判断是否有左下角点，且右边线存在，寻左线，切换到kGarare_In_l
    {
        if (GarageFindCorner() == 2)
            track_type = kTrackRight;
            break;
    }
    case kGarage_End_l://出现两个角点，停车
    {
        if (GarageFindCorner() == 3)
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

uint8 GarageFindCorner(void)
{
    bool garage_find_l = false, garage_find_r = false;

    for (int i = 0; i < l_line_count; i++)
    {
        if (l_angle_1[i] == 0) continue;
        if (garage_find_l == false && fabs(l_angle_1[i]) > 70 * 3.14 / 180 && fabs(l_angle_1[i]) < 120 * 3.14 / 180)
            garage_find_l = true;
    }
    for (int i = 0; i < r_line_count; i++)
    {
        if (r_angle_1[i] == 0) continue;
        if (garage_find_r == false && fabs(r_angle_1[i]) > 70 * 3.14 / 180 && fabs(r_angle_1[i]) < 120 * 3.14 / 180)
            garage_find_r = true;
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



