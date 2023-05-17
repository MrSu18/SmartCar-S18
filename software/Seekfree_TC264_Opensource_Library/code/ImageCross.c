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
}CrossType;//十字状态机状态结构体

CrossType cross_type = kCrossBegin;
float origin_aimdis = 0;
uint8 now_flag = 0;

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
    if(now_flag == 0)
    {
        origin_aimdis = aim_distance;
        now_flag = 1;
    }
    switch (cross_type)
    {
         //识别有角点，且边线向左右两边分开，则更改预瞄点，当角点离得很近的时候表示已经要进入十字，切换状态
        case kCrossBegin:
        {
            int16 corner_id_l = 0, corner_id_r = 0;         //角点在边线的第几个点
            if (CrossFindCorner(&corner_id_l, &corner_id_r) != 0)
            {
                //角点太远，跳出状态
                if((corner_id_l > origin_aimdis / SAMPLE_DIST) || (corner_id_r > origin_aimdis / SAMPLE_DIST))
                {
                    aim_distance = origin_aimdis;
                    break;
                }
                //左边没有角点，右边有角点，寻右线，更改预瞄点
                else if ((corner_id_l == 0) && (corner_id_r != 0))
                {
                    track_type = kTrackRight;
                    aim_distance = (float)(corner_id_r) * SAMPLE_DIST;
                }
                //右边没有角点，左边有角点，寻左线，更改预瞄点
                else if ((corner_id_l != 0) && (corner_id_r == 0))
                {
                    track_type = kTrackLeft;
                    aim_distance = (float)(corner_id_l)* SAMPLE_DIST;
                }
                //两边都有角点，寻右线，更改预瞄点
                else
                {
                    track_type = kTrackRight;
                    aim_distance = (float)(corner_id_r) * SAMPLE_DIST;
                }
                //角点很近，切换下一个状态
                if (corner_id_l < 6 && corner_id_r < 6)
                {
                    encoder_dis_flag = 1;
                    cross_type = kCrossIn;
                    aim_distance = origin_aimdis;
                }
            }
            break;
        }
        //默认重新扫线并求局部曲率最大值，通过角点判断寻那一边的边线，编码器积分到阈值或者没重新扫线就已经有边线则退出状态
        case kCrossIn:
        {
            uint8 change_lr_flag = 0;//切换寻找左右边线角点的标志位，默认左线找角点，没找到则从右线找
            if(l_line_count > 100 && r_line_count > 100)
            {
                encoder_dis_flag = 0;
                cross_type = kCrossBegin;
                aim_distance = origin_aimdis;
                return 1;
            }

            EdgeDetection_Cross('l');//左边重新扫线
            per_l_line_count=PER_EDGELINE_LENGTH;//重置透视后左线的长度
            EdgeLinePerspective(left_line,l_line_count,per_left_line);//边线逆透视
            BlurPoints(per_left_line, l_line_count, f_left_line, LINE_BLUR_KERNEL);//边线滤波
            ResamplePoints(f_left_line, l_line_count, f_left_line1, &per_l_line_count, SAMPLE_DIST*PIXEL_PER_METER);//等距采样
            local_angle_points(f_left_line1,per_l_line_count,l_angle,ANGLE_DIST/SAMPLE_DIST);//边线曲率
            nms_angle(l_angle,per_l_line_count,l_angle_1,(ANGLE_DIST/SAMPLE_DIST)*2+1);//极大值抑制
            //如果右边也没找到角点，为了防止出错，所以默认寻左线，如果右边找到角点就会寻右线
            track_leftline(f_left_line1, per_l_line_count , center_line_l, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
            track_type = kTrackLeft;
            //找左线角点
            for (int i = 0; i < per_l_line_count; i++)
            {
                //找到角点则寻左线，不扫右线，改变预瞄点
                if ((fabs(l_angle_1[i]) > 70 * 3.14 / 180) && (fabs(l_angle_1[i]) < 120 * 3.14 / 180))
                {
                    if(i > 70) break;//角点很远。跳出
                    else if((i + 6) < origin_aimdis / SAMPLE_DIST)
                    {
                        aim_distance = origin_aimdis;
                        change_lr_flag = 1;                             //等于1则不扫右线
                        break;
                    }
                    else
                    {
                        aim_distance = (float)((i + 6) * SAMPLE_DIST);
                        change_lr_flag = 1;                             //等于1则不扫右线
                        break;
                    }
                }
            }

            //左线没找到角点，从右线找
            if (change_lr_flag == 0)
            {
                EdgeDetection_Cross('r');
                per_r_line_count=PER_EDGELINE_LENGTH;//重置透视后右线的长度
                EdgeLinePerspective(right_line,r_line_count,per_right_line);//边线逆透视
                BlurPoints(per_right_line, r_line_count, f_right_line, LINE_BLUR_KERNEL);//边线滤波
                ResamplePoints(f_right_line, r_line_count, f_right_line1, &per_r_line_count, SAMPLE_DIST*PIXEL_PER_METER);//边线等距采样
                local_angle_points(f_right_line1,per_r_line_count,r_angle,ANGLE_DIST/SAMPLE_DIST);//求边线曲率
                nms_angle(r_angle,per_r_line_count,r_angle_1,(ANGLE_DIST/SAMPLE_DIST)*2+1);//曲率极大值抑制
                //找右线角点
                for (int i = 0; i < per_r_line_count; i++)
                {
                    //找到角点则寻左线，改变预瞄点
                    if ((fabs(r_angle_1[i]) > 70 * 3.14 / 180) && (fabs(r_angle_1[i]) < 120 * 3.14 / 180))
                    {
                        if(i > 70) break;//角点很远。跳出
                        else
                        {
                            track_rightline(f_right_line1, per_r_line_count , center_line_r, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
                            track_type = kTrackRight;
                            aim_distance = (float)((i + 6) * SAMPLE_DIST);
                            break;
                        }
                        if((i + 6) < origin_aimdis / SAMPLE_DIST) aim_distance = origin_aimdis;
                    }
                }
            }

            if(dis > 450)
            {
                encoder_dis_flag = 0;
                aim_distance = origin_aimdis;
                cross_type = kCrossBegin;
                return 1;
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
                cross_type = kCrossBegin;//复位状态机
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
    uint8 cross_find_l = FALSE, cross_find_r = FALSE;//是否找到角点标志位

    //找左角点
    for (int16 i = 0; i < per_l_line_count; i++)
    {
        if (cross_find_l == FALSE &&((fabs(l_angle_1[i]) > 70 * 3.14 / 180) && (fabs(l_angle_1[i]) < 120 * 3.14 / 180)))
        {
            *corner_id_l = i;//保存角点在边线第几个点
            cross_find_l = TRUE;//找到角点标志位
            break;
        }
    }
    //找右角点
    for (int16 i = 0; i < per_r_line_count; i++)
    {
        if (cross_find_r == FALSE && ((fabs(r_angle_1[i]) > 70 * 3.14 / 180) && (fabs(r_angle_1[i]) < 120 * 3.14 / 180)))
        {
            *corner_id_r = i;//保存角点在边线第几个点
            cross_find_r = TRUE;//找到角点标志位
            break;
        }
    }

    //如果两边都找到角点，返回1，一边找到角点且另一边丢线，返回2，否则返回0
    if (cross_find_l == TRUE && cross_find_r == TRUE)
    {
        if((f_left_line1[per_l_line_count - 1].X < f_left_line1[*corner_id_l].X) || (f_right_line1[per_r_line_count - 1].X > f_right_line1[*corner_id_r].X))
            return 1;
    }
    else if (cross_find_l == TRUE && cross_find_r == FALSE)
    {
        if(f_left_line1[per_l_line_count - 1].X < f_left_line1[*corner_id_l].X)
            return 2;
    }
    else if (cross_find_r == TRUE && cross_find_l == FALSE)
    {
        if(f_right_line1[per_r_line_count - 1].X > f_right_line1[*corner_id_r].X)
            return 3;
    }

    return 0;
}

/***********************************************
* @brief : 十字扫线
* @param : void
* @return: void
* @date  : 2023.4.14
* @author: L
************************************************/
void EdgeDetection_Cross(uint8 lr_flag)
{
    uint8 half=GRAY_BLOCK/2;
    switch(lr_flag)
    {
        case 'l':
        {
            myPoint left_seed = left_line[l_line_count - 1];//左种子
            int now_point = 1,last_point = 1;//保存本次的点和上一次的点是否为边缘的点
            //防止种子在边界，导致sobel检测越界
            if (left_seed.Y<USE_IMAGE_H_MIN+half || left_seed.Y>USE_IMAGE_H-half-1 || left_seed.X<half || left_seed.X>USE_IMAGE_W-half-1)
            {
                left_seed.Y=USE_IMAGE_H_MAX-half-1;left_seed.X=half;
            }
            left_seed.X++;
            //从种子开始向上寻找边缘点
            for (; left_seed.Y > USE_IMAGE_H_MAX - 45; left_seed.Y--)
            {
                last_point = now_point;
                now_point = PointSobelTest(left_seed);
                if (now_point == 1 && last_point == 0)
                    break;
            }
            l_line_count = 0;//清空之前的线

            uint8 len=EDGELINE_LENGTH-(USE_IMAGE_H_MAX-left_seed.Y);//重新扫线的长度
            //左边线生长
            while(l_line_count < len)
            {
                if (EightAreasSeedGrownGray(&left_seed,'l',&left_seed_num) == 1)
                {
                    left_line[l_line_count].X = left_seed.X;left_line[l_line_count].Y = left_seed.Y;
                    l_line_count++;
                }
                else
                    break;
            }
            break;
        }
        case 'r':
        {
            myPoint right_seed = right_line[r_line_count - 1];//右种子
            int now_point = 1,last_point = 1;//保存本次的点和上一次的点是否为边缘的点
            //防止种子在边界，导致sobel检测越界
            if (right_seed.Y<USE_IMAGE_H_MIN+half || right_seed.Y>USE_IMAGE_H-half-1 || right_seed.X<half || right_seed.X>USE_IMAGE_W-half-1)
            {
                right_seed.Y=USE_IMAGE_H_MAX-half-1,right_seed.X=USE_IMAGE_W-half-1;
            }
            right_seed.X--;
            //从种子开始向上寻找边缘点
            for (; right_seed.Y > USE_IMAGE_H_MAX - 45; right_seed.Y--)
            {
                last_point=now_point;
                now_point=PointSobelTest(right_seed);
                if (now_point == 1 && last_point == 0)
                    break;
            }
            r_line_count = 0;//清空之前的线

            uint8 len=EDGELINE_LENGTH-(USE_IMAGE_H_MAX-right_seed.Y);//重新扫线的长度
            //右边线生长
            while(r_line_count < len)
            {
                if (EightAreasSeedGrownGray(&right_seed,'r',&right_seed_num) == 1)
                {
                    right_line[r_line_count].X = right_seed.X;right_line[r_line_count].Y = right_seed.Y;
                    r_line_count++;
                }
                else
                    break;
            }
            break;
        }
    }
}
