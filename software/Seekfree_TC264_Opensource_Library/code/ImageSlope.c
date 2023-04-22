/*
 * ImageSlope.c
 *
 *  Created on: 2023年4月16日
 *      Author: L
 */
#include "zf_common_headfile.h"
#include "ImageTrack.h"
#include "motor.h"
#include "ImageSpecial.h"

typedef enum SlopeType
{
    kSlopeBegin = 0,
    kSlopeEnd,
}SlopeType;

SlopeType slope_type = kSlopeBegin;

/***********************************************
* @brief : 坡道状态机
* @param : void
* @return: 0:不是坡道
*          1:识别到坡道
* @date  : 2023.4.21
* @author: L
************************************************/
uint8 SlopeIdentify(void)
{
    switch(slope_type)
    {
        case kSlopeBegin:
        {
            gpio_toggle_level(P20_9);
            float gradient_l = 0;
            float gradient_r = 0;
            gradient_l = CalculateGradient('l');
            gradient_r = CalculateGradient('r');
            if((gradient_l * gradient_r < 0)&&(fabs(gradient_l - 1) < 0.2)&&(fabs(gradient_r - 1) < 0.2))
            {
//                if(fabs(fabs(gradient_l) - fabs(gradient_r)) < 0.3)
//                {
                    base_speed = 140;
                    last_track_mode = track_mode;
                    track_mode = kTrackADC;
                    slope_type = kSlopeEnd;
                    encoder_dis_flag = 1;
//                }
            }
            break;
        }
        case kSlopeEnd:
        {
            gpio_toggle_level(P21_5);
            if(dis >= 1000)
            {
                encoder_dis_flag = 0;
                base_speed = 150;
                last_track_mode = track_mode;
                track_mode = kTrackImage;
                slope_type = kSlopeBegin;
                return 1;
            }
            break;
        }
        default:break;
    }
    return 0;
}

float CalculateGradient(uint8 lr_flag)
{
    switch(lr_flag)
    {
        case 'l':
        {
            float gradient = 0;
            int corner_appear = 0;
            for(int i = 0;i < per_l_line_count;i++)
            {
                if(fabs(l_angle_1[i] > 0.3))
                {
                    corner_appear = i;
                    break;
                }
            }
            if(corner_appear != 0)
            {
                float dy = f_left_line1[corner_appear].Y - f_left_line1[per_l_line_count - 1].Y;
                float dx = f_left_line1[corner_appear].X - f_left_line1[per_l_line_count - 1].X;
                if(dx != 0) gradient = dy/dx;
                return gradient;
            }
            else break;
        }
        case 'r':
        {
            float gradient = 0;
            int corner_appear = 0;
            for(int i = 0;i < per_r_line_count;i++)
            {
                if(fabs(r_angle_1[i] > 0.3))
                {
                    corner_appear = i;
                    break;
                }
            }
            if(corner_appear != 0)
            {
                float dy = f_right_line1[corner_appear].Y - f_right_line1[per_r_line_count - 1].Y;
                float dx = f_right_line1[corner_appear].X - f_right_line1[per_r_line_count - 1].X;
                if(dx != 0) gradient = dy/dx;
                return gradient;
            }
            else break;
        }
    }
    return 0;
}
