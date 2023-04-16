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
    kSlopeDown,
    kSlopeEnd,
}SlopeType;

SlopeType slope_type = kSlopeBegin;

/***********************************************
* @brief : 坡道状态机\
* @param : void
* @return: 0:不是坡道
*          1:识别到坡道
* @date  : 2023.4.16
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
            if((gradient_l * gradient_r < 0)&&(fabs(gradient_l - 1) > 0.2)&&(fabs(gradient_r - 1) > 0.2))
            {
                if(fabs(fabs(gradient_l) - fabs(gradient_r)) < 0.3)
                {
                    base_speed = 140;
                    track_mode = kTrackADC;
                    slope_type = kSlopeDown;
                }
            }
            break;
        }
        case kSlopeDown:
        {
            gpio_toggle_level(P21_5);
            int numl = 0;
            int numr = 0;
            if(per_l_line_count > 20)
            {
                for(int i = 0;i < per_l_line_count - 1;i++)
                    if(fabs(l_angle_1[i]) < 10e-8) numl++;
            }
            if(per_r_line_count > 20)
            {
                for(int i = 0;i < per_r_line_count - 1;i++)
                    if(fabs(r_angle_1[i]) < 10e-8) numr++;
            }
            if(numl+numr >= (per_l_line_count+per_r_line_count-2)) slope_type = kSlopeEnd;
            break;
        }
        case kSlopeEnd:
        {
            gpio_toggle_level(P21_4);
            int num = 0;
            for(int i = 0;i < 6;i++)
            {
                if((f_right_line1[i].X - f_left_line1[i].X) < 50) num++;
            }
            if(num >= 5)
            {
                base_speed = 160;
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
                float dy = per_left_line[corner_appear].Y - per_left_line[per_l_line_count - 1].Y;
                float dx = per_left_line[corner_appear].X - per_left_line[per_l_line_count - 1].X;
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
                float dy = per_right_line[corner_appear].Y - per_right_line[per_r_line_count - 1].Y;
                float dx = per_right_line[corner_appear].X - per_right_line[per_r_line_count - 1].X;
                if(dx != 0) gradient = dy/dx;
                return gradient;
            }
            else break;
        }
    }
    return 0;
}
