/*
 * ImageCut.c
 *
 *  Created on: 2023年3月20日
 *      Author: L
 */
#include "motor.H"
#include "ImageSpecial.h"
#include "ImageTrack.h"
#include "ImageBasic.h"
#include "ImageProcess.h"

typedef enum Cut_Type
{
    CUT_IN = 0,
    CUT_OUT,
}Cut_Type;

Cut_Type cut_type = CUT_IN;

/***********************************************
* @brief : 断路状态机
* @param : void
* @return: 0:不是十断路
*          1:识别到断路
* @date  : 2023.3.11
* @author: L
************************************************/
uint8 CutIdentify(void)
{
    switch (cut_type)
    {
    case CUT_IN:
    {
        int  corner_id_l = 0, corner_id_r = 0;
        if (CrossFindCorner(&corner_id_l, &corner_id_r) == 1 && (f_left_line[corner_id_l + 4].X > f_left_line[corner_id_l].X || f_right_line[corner_id_r + 4].X < f_right_line[corner_id_r].X))
        {
            if (l_line_count + r_line_count < 15)
            {
                while(1)
                {
                    base_speed = 0;
                    image_bias = 0;
                }
                cut_type = CUT_OUT;
                aim_distance = 0.32;
            }
            else
            {
                if (corner_id_l == 0 && corner_id_r != 0)
                    aim_distance = (float)corner_id_r * SAMPLE_DIST;
                else if (corner_id_l != 0 && corner_id_r == 0)
                    aim_distance = (float)corner_id_l * SAMPLE_DIST;
                else
                    aim_distance = ((float)(corner_id_l + corner_id_r)) * SAMPLE_DIST / 2;
            }
        }
        break;
    }
    case CUT_OUT:
    {
        track_mode = kTrackADC;
        if (l_line_count > 10 || r_line_count > 10)
        {
//            gpio_set_level(P21_4,0);
            cut_type = CUT_IN;
            track_mode = kTrackImage;
            return 1;
        }
        break;
    }
    default:break;
    }
    return 0;
}
