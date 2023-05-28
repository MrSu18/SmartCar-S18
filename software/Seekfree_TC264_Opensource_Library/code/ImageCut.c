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
#include "Control.h"

typedef enum CutType
{
    kCutBegin=0,
    kCutIn,
    kCutMid,
    kCutEnd,
}CutType;//断路状态机状态结构体

CutType cut_type = kCutBegin;

/***********************************************
* @brief : 断路状态机
* @param : void
* @return: 0:不是断路
*          1:识别到断路
* @date  : 2023.5.26
* @author: L
************************************************/
uint8 CutIdentify(void)
{
    if(now_flag == 0)
    {
        origin_aimdis = aim_distance;
        now_flag = 1;
    }
    switch(cut_type)
    {
        case kCutBegin:
        {
            int16 corner_id_l = 0,corner_id_r = 0;
            if(CutFindCorner(&corner_id_l, &corner_id_r)!=0)
            {
                if((corner_id_l > origin_aimdis / SAMPLE_DIST) || (corner_id_r > origin_aimdis / SAMPLE_DIST))
                    break;
                else
                    cut_type = kCutIn;
            }
            break;
        }
        case kCutIn:
        {
//            printf("1\n");
            int16 corner_id_l = 0,corner_id_r = 0;
            uint8 corner_find = CutFindCorner(&corner_id_l, &corner_id_r);//找角点
            if(corner_find != 0)
            {
                //有右角点，没有左角点
                if(corner_find == 2)
                {
                    per_r_line_count = (int)corner_id_r;//修改边线长度到角点位置
                    track_type = kTrackRight;
                    aim_distance = (float)corner_id_r*SAMPLE_DIST;
                }
                //有左角点，没有右角点
                else if(corner_find == 1)
                {
                    per_l_line_count = (int)corner_id_l;//修改边线长度到角点位置
                    track_type = kTrackLeft;
                    aim_distance = (float)corner_id_l*SAMPLE_DIST;
                }
                //两边都有角点，那边边线长就寻那条边线
                else if(corner_find == 3)
                {
                    if(per_l_line_count > per_r_line_count)
                    {
                        per_l_line_count = (int)corner_id_l;//修改边线长度到角点位置
                        track_type = kTrackLeft;
                        aim_distance = (float)corner_id_l*SAMPLE_DIST;
                    }
                    else if(per_r_line_count > per_l_line_count)
                    {
                        per_r_line_count = (int)corner_id_r;//修改边线长度到角点位置
                        track_type = kTrackRight;
                        aim_distance = (float)corner_id_r*SAMPLE_DIST;
                    }
                }
                //切换状态，改成电磁循迹
                if(corner_id_l < 40 && corner_id_r < 40)
                {
                    speed_type=kNormalSpeed;
                    base_speed = 55;
                    last_track_mode = track_mode;
                    track_mode = kTrackADC;
                    cut_type = kCutMid;
                    aim_distance = origin_aimdis;
                }
            }
            break;
        }
        case kCutMid:
        {
//            printf("2\n");
            int over_count = 0;
            for(int16 i = 0;i < MT9V03X_W;i++)
            {
                if(mt9v03x_image[106][i] <= OUT_THRESHOLD)
                        over_count++;
            }
            if(over_count>=MT9V03X_W-2)
                cut_type = kCutEnd;
            break;
        }
        case kCutEnd:
        {
//            printf("3\n");
            //边线重新出现，断路状态结束，切换成图像循迹
            if (l_line_count>80 && r_line_count>80)
            {
//                printf("4\n");
                last_track_mode = track_mode;
                track_mode = kTrackImage;
                cut_type = kCutBegin;//复位状态机
                base_speed = original_speed;
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
    //左角点
    for(int i = 0;i < per_l_line_count;i++)
    {
        if((fabs(l_angle_1[i]) > 70 * 3.14 / 180) && (fabs(l_angle_1[i]) < 120 * 3.14 / 180))
        {
            *corner_id_l = (int16)i;
            break;
        }
    }
    //右角点
    for(int i = 0;i < per_r_line_count;i++)
    {
        if((fabs(r_angle_1[i]) > 70 * 3.14 / 180) && (fabs(r_angle_1[i]) < 120 * 3.14 / 180))
        {
            *corner_id_r = (int16)i;
            break;
        }
    }
    //只找到左角点
    if(*corner_id_l != 0 && *corner_id_r == 0)
    {
        if(f_left_line1[per_l_line_count - 1].X > f_left_line1[*corner_id_l].X)
            return 1;
    }
    //只找到右角点
    else if(*corner_id_l == 0 && *corner_id_r != 0)
    {
        if(f_right_line1[per_r_line_count - 1].X < f_right_line1[*corner_id_r].X)
            return 2;
    }
    //两边都有角点
    else if(*corner_id_l != 0 && *corner_id_r != 0)
    {
        if((f_left_line1[per_l_line_count - 1].X > f_left_line1[*corner_id_l].X) || (f_right_line1[per_r_line_count - 1].X < f_right_line1[*corner_id_r].X))
            return 3;
    }
    //没找到角点
    return 0;
}

