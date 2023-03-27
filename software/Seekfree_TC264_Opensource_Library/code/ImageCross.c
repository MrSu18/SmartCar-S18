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
     //识别有角点，且边线向左右两边分开，则更改预瞄点，当角点离得很近的时候表示已经要进入十字，切换状态
    case kCrossBegin:
    {
        int16 corner_id_l = 0, corner_id_r = 0;         //角点在边线的第几个点
        if ((CrossFindCorner(&corner_id_l, &corner_id_r) != 0) && (f_left_line[l_line_count - 1].X < f_left_line[corner_id_l].X || f_right_line[r_line_count - 1].X > f_right_line[corner_id_r].X))
        {
            //如果只有一边有角点，则不用求平均值，如果有两个角点，则求平均值
            if ((corner_id_l == 0) && (corner_id_r != 0))
                aim_distance = (float)corner_id_r * SAMPLE_DIST;
            else if ((corner_id_l != 0) && (corner_id_r == 0))
                aim_distance = (float)corner_id_l * SAMPLE_DIST;
            else
                aim_distance = ((float)(corner_id_l + corner_id_r)) * SAMPLE_DIST / 2;

            //角点很近，切换下一个状态
            if (corner_id_l < 8 && corner_id_r < 8)
                cross_type = kCrossIn;
        }
        else
            aim_distance = 0.45;
        break;
    }
    //默认重新扫线并求局部曲率最大值，通过角点判断寻那一边的边线，如果没有找到角点，则表示已经要出了十字，切换状态
    case kCrossIn:
    {
        uint8 change_lr_flag = 0;               //切换寻找左右边线角点的标志位，默认右线找角点，没找到则从左线找
        uint8 corner_find = 0;                  //是否找到角点的标志位

        EdgeDetection_Cross();

        BlurPoints(left_line, l_line_count, f_left_line, LINE_BLUR_KERNEL);
        BlurPoints(right_line, r_line_count, f_right_line, LINE_BLUR_KERNEL);

        local_angle_points(f_left_line, l_line_count, l_angle, ANGLE_DIST / SAMPLE_DIST);
        local_angle_points(f_right_line, r_line_count, r_angle, ANGLE_DIST / SAMPLE_DIST);

        nms_angle(l_angle, l_line_count, l_angle_1, (ANGLE_DIST / SAMPLE_DIST) * 2 + 1);
        nms_angle(r_angle, r_line_count, r_angle_1, (ANGLE_DIST / SAMPLE_DIST) * 2 + 1);

        //找右线角点
        for (int i = 0; i < r_line_count; i++)
        {
            //找到角点则寻右线，不寻左线，改变预瞄点
            if ((fabs(r_angle_1[i]) > 70 * 3.14 / 180) && (fabs(r_angle_1[i]) < 120 * 3.14 / 180))
            {
                track_rightline(f_right_line, r_line_count, center_line_r, (int)round(ANGLE_DIST / SAMPLE_DIST), PIXEL_PER_METER * (TRACK_WIDTH / 2));
                track_type = kTrackRight;
                aim_distance = (float)i * SAMPLE_DIST;
                change_lr_flag = 1;                             //等于1则不从左线找角点
                corner_find = 1;                                //等于0是默认寻右线，这里是寻右线
                break;
            }
        }
        //右线没找到角点，从左线找
        if (change_lr_flag == 0)
        {
            for (int i = 0; i < l_line_count; i++)
            {
                //找到角点则寻左线，改变预瞄点
                if ((fabs(r_angle_1[i]) > 70 * 3.14 / 180) && (fabs(r_angle_1[i]) < 120 * 3.14 / 180))
                {
                    track_leftline(f_left_line, l_line_count, center_line_l, (int)round(ANGLE_DIST / SAMPLE_DIST), PIXEL_PER_METER * (TRACK_WIDTH / 2));
                    track_type = kTrackLeft;
                    aim_distance = (float)i * SAMPLE_DIST;
                    corner_find = 1;                                //等于0是默认寻右线，这里是寻左线线
                    break;
                }
            }
        }
        //没有找到角点，默认寻右线，切换下一个状态
        if (corner_find == 0)
        {
            track_rightline(f_right_line, r_line_count, center_line_r, (int)round(ANGLE_DIST / SAMPLE_DIST), PIXEL_PER_METER * (TRACK_WIDTH / 2));
            track_type = kTrackRight;
            cross_type = kCrossOut;
        }

        break;
    }
    //判断是否已经出了十字
    case kCrossOut:
    {
        //当左右边线都大于10时，确认已经出了十字，退出状态机，状态机复位
        if (l_line_count > 10 && r_line_count > 10)
        {
            aim_distance = 0.45;
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
    uint8 cross_find_l = FALSE, cross_find_r = FALSE;                       //是否找到角点标志位

    //找左角点
    for (int16 i = 0; i < l_line_count; i++)
    {
        if (cross_find_l == FALSE &&((fabs(l_angle_1[i]) > 70 * 3.14 / 180) && (fabs(l_angle_1[i]) < 120 * 3.14 / 180)))
        {
            *corner_id_l = i;
            cross_find_l = TRUE;
            break;
        }
    }
    //找右角点
    for (int i = 0; i < r_line_count; i++)
    {
        if (cross_find_r == FALSE && ((fabs(r_angle_1[i]) > 70 * 3.14 / 180) && (fabs(r_angle_1[i]) < 120 * 3.14 / 180)))
        {
            cross_find_r = TRUE;
            break;
        }
    }

    //如果两边都找到角点，返回1，一边找到角点且另一边丢线，返回2，否则返回0
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
    myPoint left_seed, right_seed;              //左右种子
    left_seed = left_line[l_line_count - 1]; right_seed = right_line[r_line_count - 1];
    l_line_count = 0; r_line_count = 0;         //对原边线清零
    uint8 change_lr_flag = 0, left_finish = 0, right_finish = 0;//change_lr_flag=0:左边生长 change_lr_flag=1:右边生长
    uint8 is_loseline = 0;                      //是否丢线的标志位，丢线则继续生长，不丢线就计入边线
    do
    {
        if (change_lr_flag == 0 && left_finish == 0)
        {
            is_loseline = EightAreasSeedGrown(&left_seed, 'l', &left_seed_num);
            //丢线，不计入边线
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
            //找到边线
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
            //丢线，不计入边线
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
            //找到边线
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

