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
    switch (cut_type)
    {
    case kCutIn:
    {
        int16 corner_id[2] = { 0 };
        uint8 r_corner_result = CutFindCorner(corner_id, 'r');

        if (r_corner_result == 2 || r_corner_result == 1)
        {
            if (f_right_line[corner_id[0]].X > f_right_line[r_line_count - 1].X)
            {
                r_line_count = (uint8)corner_id[0];
                track_rightline(f_right_line, r_line_count, center_line_r, (int)round(ANGLE_DIST / SAMPLE_DIST), PIXEL_PER_METER * (TRACK_WIDTH / 2));
                track_type = kTrackRight;
                aim_distance = (float)corner_id[0] * SAMPLE_DIST;
            }
            if (corner_id[0] < 8)
            {
                last_track_mode = track_mode;
                track_mode = kTrackADC;
                cut_type = kCutOut;
                aim_distance = 0.32;
            }
        }
        else if (r_corner_result == 0)
        {
            uint8 l_corner_result = CutFindCorner(corner_id, 'l');

            if (l_corner_result == 2 || l_corner_result == 1)
            {
                if (f_left_line[corner_id[0]].X < f_left_line[l_line_count - 1].X)
                {
                    l_line_count = (uint8)corner_id[0];
                    track_leftline(f_left_line, l_line_count, center_line_l, (int)round(ANGLE_DIST / SAMPLE_DIST), PIXEL_PER_METER * (TRACK_WIDTH / 2));
                    track_type = kTrackLeft;
                    aim_distance = (float)corner_id[0] * SAMPLE_DIST;
                }
                if (corner_id[0] < 8)
                {
                    last_track_mode = track_mode;
                    track_mode = kTrackADC;
                    cut_type = kCutOut;
                    aim_distance = 0.32;
                }
            }
        }
        break;
    }
    case kCutOut:
    {
        if (l_line_count > 20 && r_line_count > 20)
        {
            last_track_mode = track_mode;
            track_mode = kTrackImage;
            cut_type = kCutIn;
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
* @date  : 2023.3.23
* @author: L
************************************************/
uint8 CutFindCorner(int16 corner_id[2],uint8 lr_flag)
{
    uint8 corner_count = 0;
    switch (lr_flag)
    {
    case 'l':
    {
        for (int16 i = 0; i < l_line_count; i++)
        {
            if ((fabs(l_angle_1[i]) > 70 * 3.14 / 180) && (fabs(l_angle_1[i]) < 120 * 3.14 / 180))
            {
                corner_id[corner_count] = i;
                corner_count++;
            }
        }
        break;
    }
    case 'r':
    {
        for (int16 i = 0; i < r_line_count; i++)
        {
            if ((fabs(r_angle_1[i]) > 70 * 3.14 / 180) && (fabs(r_angle_1[i]) < 120 * 3.14 / 180))
            {
                corner_id[corner_count] = i;
                corner_count++;
            }
        }
        break;
    }
    default:break;
    }
    return corner_count;
}
