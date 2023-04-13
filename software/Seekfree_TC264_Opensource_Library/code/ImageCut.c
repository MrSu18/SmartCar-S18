/*
 * ImageCut.c
 *
 *  Created on: 2023��3��20��
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
* @brief : ��·״̬��
* @param : void
* @return: 0:����ʮ��·
*          1:ʶ�𵽶�·
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
            if(CutFindCorner(&corner_id_l, &corner_id_r) != 0)
            {
                gpio_toggle_level(P20_9);
                base_speed = 140;
                if((corner_id_l == 0)&&(corner_id_r != 0))
                {
                    r_line_count = (uint8)corner_id_r;
                    track_rightline(f_right_line, r_line_count, center_line_r, (int)round(ANGLE_DIST / SAMPLE_DIST), PIXEL_PER_METER * (TRACK_WIDTH / 2));
                    track_type = kTrackRight;
                    aim_distance = (float)corner_id_r*SAMPLE_DIST;
                }
                else if((corner_id_l != 0)&&(corner_id_r == 0))
                {
                    l_line_count = (uint8)corner_id_l;
                    track_leftline(f_left_line, l_line_count, center_line_l, (int)round(ANGLE_DIST / SAMPLE_DIST), PIXEL_PER_METER * (TRACK_WIDTH / 2));
                    track_type = kTrackLeft;
                    aim_distance = (float)corner_id_l*SAMPLE_DIST;
                }
                else if((corner_id_l != 0)&&(corner_id_r != 0))
                {
                    r_line_count = (uint8)(corner_id_l + corner_id_r)/2;
                    track_rightline(f_right_line, r_line_count, center_line_r, (int)round(ANGLE_DIST / SAMPLE_DIST), PIXEL_PER_METER * (TRACK_WIDTH / 2));
                    track_type = kTrackRight;
                    aim_distance = (float)(corner_id_l + corner_id_r)/2.0;
                }

                if(corner_id_l < 20 && corner_id_r <20)
                {
                    last_track_mode = track_mode;
                    track_mode = kTrackADC;
                    cut_type = kCutOut;
                    aim_distance = 0.36;
                }
            }
            else
                aim_distance = 0.36;
            break;
        }
        case kCutOut:
        {
            gpio_toggle_level(P21_5);
            if (l_line_count > 35 && r_line_count > 35)
            {
                gpio_toggle_level(P21_4);
                last_track_mode = track_mode;
                track_mode = kTrackImage;
                cut_type = kCutIn;
                base_speed = 160;
                return 1;
            }
            break;
        }
        default:break;
    }
    return 0;
}
//uint8 CutIdentify(void)
//{
//    switch (cut_type)
//    {
//    case kCutIn:
//    {
//        int16 corner_id[2] = { 0 };
//        uint8 r_corner_result = CutFindCorner(corner_id, 'r');
//
//        if (r_corner_result == 2 || r_corner_result == 1)
//        {
//            gpio_toggle_level(P20_9);
//            if (f_right_line[corner_id[0]].X > f_right_line[r_line_count - 1].X)
//            {
//                base_speed = 140;
//                r_line_count = (uint8)corner_id[0];
//                track_rightline(f_right_line, r_line_count, center_line_r, (int)round(ANGLE_DIST / SAMPLE_DIST), PIXEL_PER_METER * (TRACK_WIDTH / 2));
//                track_type = kTrackRight;
//                aim_distance = (float)corner_id[0] * SAMPLE_DIST;
//            }
//            if (corner_id[0] < 20)
//            {
//                last_track_mode = track_mode;
//                track_mode = kTrackADC;
//                cut_type = kCutOut;
//                aim_distance = 0.36;
//            }
//        }
//        else if (r_corner_result == 0)
//        {
//            uint8 l_corner_result = CutFindCorner(corner_id, 'l');
//
//            if (l_corner_result == 2 || l_corner_result == 1)
//            {
//                gpio_toggle_level(P21_5);
//                if (f_left_line[corner_id[0]].X < f_left_line[l_line_count - 1].X)
//                {
//                    base_speed = 140;
//                    l_line_count = (uint8)corner_id[0];
//                    track_leftline(f_left_line, l_line_count, center_line_l, (int)round(ANGLE_DIST / SAMPLE_DIST), PIXEL_PER_METER * (TRACK_WIDTH / 2));
//                    track_type = kTrackLeft;
//                    aim_distance = (float)corner_id[0] * SAMPLE_DIST;
//                }
//                if (corner_id[0] < 25)
//                {
//                    last_track_mode = track_mode;
//                    track_mode = kTrackADC;
//                    cut_type = kCutOut;
//                    aim_distance = 0.36;
//                }
//            }
//        }
//        break;
//    }
//    case kCutOut:
//    {
//        gpio_toggle_level(P21_4);
//        if (l_line_count > 35 && r_line_count > 35)
//        {
//            last_track_mode = track_mode;
//            track_mode = kTrackImage;
//            cut_type = kCutIn;
//            base_speed = 160;
//            return 1;
//        }
//        break;
//    }
//    default:break;
//    }
//    return 0;
//}

/***********************************************
* @brief : ��·Ѱ�ҽǵ�
* @param : corner_id[2]:��ȡ�ǵ��ڱ��ߵĵڼ�����
* @return: corner_count:�ǵ������
* @date  : 2023.3.23
* @author: L
************************************************/
uint8 CutFindCorner(int16* corner_id_l,int16* corner_id_r)
{
    for(int i = 0;i < l_line_count;i++)
    {
        if((fabs(l_angle_1[i]) > 70 * 3.14 / 180) && (fabs(l_angle_1[i]) < 120 * 3.14 / 180))
        {
            *corner_id_l = (int16)i;
            break;
        }
    }
    for(int i = 0;i < r_line_count;i++)
    {
        if((fabs(r_angle_1[i]) > 70 * 3.14 / 180) && (fabs(r_angle_1[i]) < 120 * 3.14 / 180))
        {
            *corner_id_r = (int16)i;
            break;
        }
    }
    if(corner_id_l != 0 && corner_id_r == 0)
    {
        if((f_left_line[l_line_count - 1].X > f_left_line[*corner_id_l].X) || (f_right_line[r_line_count - 1].X < f_right_line[*corner_id_r].X))
            return 1;
    }

    return 0;
}
//uint8 CutFindCorner(int16 corner_id[2],uint8 lr_flag)
//{
//    uint8 corner_count = 0;
//    switch (lr_flag)
//    {
//    case 'l':
//    {
//        for (int16 i = 0; i < l_line_count; i++)
//        {
//            if ((fabs(l_angle_1[i]) > 70 * 3.14 / 180) && (fabs(l_angle_1[i]) < 120 * 3.14 / 180))
//            {
//                corner_id[corner_count] = i;
//                corner_count++;
//            }
//        }
//        break;
//    }
//    case 'r':
//    {
//        for (int16 i = 0; i < r_line_count; i++)
//        {
//            if ((fabs(r_angle_1[i]) > 70 * 3.14 / 180) && (fabs(r_angle_1[i]) < 120 * 3.14 / 180))
//            {
//                corner_id[corner_count] = i;
//                corner_count++;
//            }
//        }
//        break;
//    }
//    default:break;
//    }
//    return corner_count;
//}
