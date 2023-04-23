/*
 * ImageCut.c
 *
 *  Created on: 2023年3月20日
 *      Author: L
 */
#include "motor.h"
#include <math.h>
#include "ImageSpecial.h"
#include "ImageTrack.h"
#include "ImageBasic.h"
#include "ImageProcess.h"

typedef enum CutType
{
    kCutIn = 0,
    kCutOut,
}CutType;

CutType cut_type = kCutIn;

/***********************************************
* @brief : 断路状态机
* @param : void
* @return: 0:不是十断路
*          1:识别到断路
* @date  : 2023.3.23
* @author: L
************************************************/
uint8 CutIdentify(void)
{
    switch(cut_type)
    {
        case kCutIn:
        {
            int16 corner_id_l = 0,corner_id_r = 0;
            uint8 corner_find = CutFindCorner(&corner_id_l, &corner_id_r);
            if(corner_find != 0)
            {
                gpio_toggle_level(P20_9);
                base_speed = 110;
                if(corner_find == 2)
                {
                    per_r_line_count = (int)corner_id_r;
                    track_rightline(f_right_line1, per_r_line_count, center_line_r, (int)round(ANGLE_DIST / SAMPLE_DIST), PIXEL_PER_METER * (TRACK_WIDTH / 2));
                    track_type = kTrackRight;
                    aim_distance = (float)(corner_id_r/2)*SAMPLE_DIST;
                }
                else if(corner_find == 1)
                {
                    per_l_line_count = (int)corner_id_l;
                    track_leftline(f_left_line1, per_l_line_count, center_line_l, (int)round(ANGLE_DIST / SAMPLE_DIST), PIXEL_PER_METER * (TRACK_WIDTH / 2));
                    track_type = kTrackLeft;
                    aim_distance = (float)(corner_id_l/2)*SAMPLE_DIST;
                }
                else if(corner_find == 3)
                {
                    if(per_l_line_count > per_r_line_count)
                    {
                        per_l_line_count = (int)corner_id_l;
                        track_leftline(f_left_line1, per_l_line_count, center_line_l, (int)round(ANGLE_DIST / SAMPLE_DIST), PIXEL_PER_METER * (TRACK_WIDTH / 2));
                        track_type = kTrackLeft;
                        aim_distance = (float)(corner_id_l/2)*SAMPLE_DIST;
                    }
                    else if(per_r_line_count > per_l_line_count)
                    {
                        per_r_line_count = (int)corner_id_r;
                        track_rightline(f_right_line1, per_r_line_count, center_line_r, (int)round(ANGLE_DIST / SAMPLE_DIST), PIXEL_PER_METER * (TRACK_WIDTH / 2));
                        track_type = kTrackRight;
                        aim_distance = (float)(corner_id_r/2)*SAMPLE_DIST;
                    }
                }

                if(corner_id_l < 15 && corner_id_r < 15)
                {
                    last_track_mode = track_mode;
                    track_mode = kTrackADC;
                    cut_type = kCutOut;
                    aim_distance = 0.45;
                }
            }
//            else
//                aim_distance = 0.45;
            break;
        }
        case kCutOut:
        {
            gpio_toggle_level(P21_5);
            if (per_l_line_count > 50 && per_r_line_count > 50)
            {
                gpio_toggle_level(P21_4);
                last_track_mode = track_mode;
                track_mode = kTrackImage;
                cut_type = kCutIn;
                base_speed = 150;
                return 1;
            }
            break;
        }
        default:break;
    }
    return 0;
}
/***********************************************
* @brief : 断路寻找角点
* @param : corner_id[2]:存取角点在边线的第几个点
* @return: corner_count:角点的数量
* @date  : 2023.4.21
* @author: L
************************************************/
uint8 CutFindCorner(int16* corner_id_l,int16* corner_id_r)
{
    for(int i = 0;i < per_l_line_count;i++)
    {
        if((fabs(l_angle_1[i]) > 70 * 3.14 / 180) && (fabs(l_angle_1[i]) < 120 * 3.14 / 180))
        {
            *corner_id_l = (int16)i;
            break;
        }
    }
    for(int i = 0;i < per_r_line_count;i++)
    {
        if((fabs(r_angle_1[i]) > 70 * 3.14 / 180) && (fabs(r_angle_1[i]) < 120 * 3.14 / 180))
        {
            *corner_id_r = (int16)i;
            break;
        }
    }
    if(*corner_id_l != 0 && *corner_id_r == 0)
    {
        if(f_left_line1[per_l_line_count - 1].X > f_left_line1[*corner_id_l].X)
            return 1;
    }
    else if(*corner_id_l == 0 && *corner_id_r != 0)
    {
        if(f_right_line1[per_r_line_count - 1].X < f_right_line1[*corner_id_r].X)
            return 2;
    }
    else if(*corner_id_l != 0 && *corner_id_r != 0)
    {
        if((f_left_line1[per_l_line_count - 1].X > f_left_line1[*corner_id_l].X) || (f_right_line1[per_r_line_count - 1].X < f_right_line1[*corner_id_r].X))
            return 3;
    }

    return 0;
}
