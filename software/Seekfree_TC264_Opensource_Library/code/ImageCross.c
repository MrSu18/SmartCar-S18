/*
 * ImageCross.c
 *
 *  Created on: 2023年3月10日
 *      Author: L
 */
#include "ImageSpecial.h"
#include "ImageTrack.h"
#include <math.h>
#include "ImageBasic.h"
#include "ImageProcess.h"
#include "motor.h"
#include "zf_device_tft180.h"

typedef enum CrossType
{
    kCrossBegin = 0,
    kCrossIn,
    kCrossOut,
}CrossType;

CrossType cross_type = kCrossBegin;

/***********************************************
* @brief : 十字路口状态机
* @param : void
* @return: 0:不是十字
*          1:识别到十字
* @date  : 2023.3.21
* @author: L
************************************************/
uint8 CrossIdentify(void)
{
    switch (cross_type)
    {
    case kCrossBegin:
    {
        int16 corner_id_l = 0, corner_id_r = 0;
        if ((CrossFindCorner(&corner_id_l, &corner_id_r) != 0) && (f_left_line[l_line_count - 1].X < f_left_line[corner_id_l].X || f_right_line[r_line_count - 1].X > f_right_line[corner_id_r].X))
        {
            if ((corner_id_l == 0) && (corner_id_r != 0))
                aim_distance = (float)corner_id_r * SAMPLE_DIST;
            else if ((corner_id_l != 0) && (corner_id_r == 0))
                aim_distance = (float)corner_id_l * SAMPLE_DIST;
            else
                aim_distance = ((float)(corner_id_l + corner_id_r)) * SAMPLE_DIST / 2;

            if (corner_id_l < 8 && corner_id_r < 8)
                cross_type = kCrossIn;
        }
        else
            aim_distance = 0.32;
        break;
    }
    case kCrossIn:
    {
        uint8 change_lr_flag = 0;
        uint8 corner_find = 0;

        EdgeDetection_Cross();

        BlurPoints(left_line, l_line_count, f_left_line, LINE_BLUR_KERNEL);
        BlurPoints(right_line, r_line_count, f_right_line, LINE_BLUR_KERNEL);

        local_angle_points(f_left_line, l_line_count, l_angle, ANGLE_DIST / SAMPLE_DIST);
        local_angle_points(f_right_line, r_line_count, r_angle, ANGLE_DIST / SAMPLE_DIST);

        nms_angle(l_angle, l_line_count, l_angle_1, (ANGLE_DIST / SAMPLE_DIST) * 2 + 1);
        nms_angle(r_angle, r_line_count, r_angle_1, (ANGLE_DIST / SAMPLE_DIST) * 2 + 1);

        for (int i = 0; i < r_line_count; i++)
        {
            if ((fabs(r_angle_1[i]) > 70 * 3.14 / 180) && (fabs(r_angle_1[i]) < 120 * 3.14 / 180))
            {
                track_rightline(f_right_line, r_line_count, center_line_r, (int)round(ANGLE_DIST / SAMPLE_DIST), PIXEL_PER_METER * (TRACK_WIDTH / 2));
                track_type = kTrackRight;
                aim_distance = (float)i * SAMPLE_DIST;
                change_lr_flag = 1;
                corner_find = 1;
                break;
            }
        }
        if (change_lr_flag == 0)
        {
            for (int i = 0; i < l_line_count; i++)
            {
                if ((fabs(r_angle_1[i]) > 70 * 3.14 / 180) && (fabs(r_angle_1[i]) < 120 * 3.14 / 180))
                {
                    track_leftline(f_left_line, l_line_count, center_line_l, (int)round(ANGLE_DIST / SAMPLE_DIST), PIXEL_PER_METER * (TRACK_WIDTH / 2));
                    track_type = kTrackLeft;
                    aim_distance = (float)i * SAMPLE_DIST;
                    corner_find = 1;
                    break;
                }
            }
        }
        if (corner_find == 0)
        {
            track_rightline(f_right_line, r_line_count, center_line_r, (int)round(ANGLE_DIST / SAMPLE_DIST), PIXEL_PER_METER * (TRACK_WIDTH / 2));
            cross_type = kCrossOut;
        }

        break;
    }
    case kCrossOut:
    {
        if (l_line_count > 10 && r_line_count > 10)
        {
            aim_distance = 0.32;
            cross_type = kCrossBegin;
            return 1;
        }
        break;
    }
    default:break;
    }
    return 0;
}

/***********************************************
* @brief : 找十字角点
* @param : corner_id_l:左角点在第几个点
*          corner_id_r:右角点在第几个点
* @return: 0：没有角点
*          1:找到两个角点
*          2:找到一个角点，且另一边丢线
* @date  : 2023.3.21
* @author: L
************************************************/
uint8 CrossFindCorner(int16* corner_id_l, int16* corner_id_r)
{
    uint8 cross_find_l = FALSE, cross_find_r = FALSE;

    for (int16 i = 0; i < l_line_count; i++)
    {
        if (cross_find_l == FALSE &&((fabs(l_angle_1[i]) > 70 * 3.14 / 180) && (fabs(l_angle_1[i]) < 120 * 3.14 / 180)))
        {
            *corner_id_l = i;
            cross_find_l = TRUE;
            break;
        }
    }
    for (int i = 0; i < r_line_count; i++)
    {
        if (cross_find_r == FALSE && ((fabs(r_angle_1[i]) > 70 * 3.14 / 180) && (fabs(r_angle_1[i]) < 120 * 3.14 / 180)))
        {
            *corner_id_r = i;
            cross_find_r = TRUE;
            break;
        }
    }

    if (cross_find_l == TRUE && cross_find_r == TRUE)
        return 1;
    else if ((cross_find_l == TRUE && (r_line_count < 10)) || (cross_find_r == TRUE && (l_line_count < 10)))
        return 2;
    else
        return 0;
}

/***********************************************
* @brief : 十字扫线
* @param : void
* @return: void
* @date  : 2023.3.21
* @author: L
************************************************/
void EdgeDetection_Cross(void)
{
    myPoint left_seed, right_seed;
    left_seed = left_line[l_line_count - 1]; right_seed = right_line[r_line_count - 1];
    l_line_count = 0; r_line_count = 0;
    uint8 change_lr_flag = 0, left_finish = 0, right_finish = 0;//change_lr_flag=0:左边生长 change_lr_flag=1:右边生长
    uint8 is_loseline = 0;
    do
    {
        if (change_lr_flag == 0 && left_finish == 0)
        {
            is_loseline = EightAreasSeedGrown(&left_seed, 'l', &left_seed_num);
            if (is_loseline == 2)
            {
                //切换左右巡线的标志变量
                if (left_seed.Y == 0 || left_seed.X == right_border[left_seed.Y])//左种子生长到了图像上边界或右边界说明扫完了左边
                {
                    change_lr_flag = !change_lr_flag;
                    left_finish = 1;
                }
                else if (left_seed.X == left_border[left_seed.Y])//左种子生长到了左边界
                {
                    if (right_finish != 1)  change_lr_flag = !change_lr_flag;
                }
            }
            else if (is_loseline == 1)
            {
                left_line[l_line_count] = left_seed; l_line_count++;
                //切换左右巡线的标志变量
                if (left_seed.Y == 0 || left_seed.X == right_border[left_seed.Y] || l_line_count >= EDGELINE_LENGTH - 1)//左种子生长到了图像上边界或右边界说明扫完了左边
                {
                    change_lr_flag = !change_lr_flag;
                    left_finish = 1;
                }
                else if (left_seed.X == left_border[left_seed.Y])//左种子生长到了左边界
                {
                    if (right_finish != 1)  change_lr_flag = !change_lr_flag;
                }
            }
            else
            {
                change_lr_flag = !change_lr_flag;
                left_finish = 1;
            }
        }
        else if (change_lr_flag == 1 && right_finish == 0)
        {
            is_loseline = EightAreasSeedGrown(&right_seed, 'r', &right_seed_num);
            if (is_loseline == 2)
            {
                //切换左右巡线的标志变量
                if (right_seed.Y == 0 || right_seed.X == left_border[right_seed.Y])//左种子生长到了图像上边界或右边界说明扫完了左边
                {
                    change_lr_flag = !change_lr_flag;
                    right_finish = 1;
                }
                else if (right_seed.X == right_border[right_seed.Y])//左种子生长到了左边界
                {
                    if (left_finish != 1)  change_lr_flag = !change_lr_flag;
                }
            }
            else if (is_loseline == 1)
            {
                right_line[r_line_count] = right_seed; r_line_count++;
                //切换左右巡线的标志变量
                if (right_seed.Y == 0 || right_seed.X == left_border[right_seed.Y] || r_line_count >= EDGELINE_LENGTH - 1)//左种子生长到了图像上边界或右边界说明扫完了左边
                {
                    change_lr_flag = !change_lr_flag;
                    right_finish = 1;
                }
                else if (right_seed.X == right_border[right_seed.Y])//左种子生长到了左边界
                {
                    if (left_finish != 1)  change_lr_flag = !change_lr_flag;
                }
            }
            else
            {
                change_lr_flag = !change_lr_flag;
                right_finish = 1;
            }
        }
        else break;
    } while (left_seed.Y != right_seed.Y || left_seed.X != right_seed.X);
}

