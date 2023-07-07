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
#include "motor.h"
#include "debug.h"
#include "zf_driver_gpio.h"

typedef enum CutType
{
    kCutBegin=0,
    kCutIn,
    kCutMid,
    kCutEndL,
    kCutEndR,
}CutType;//断路状态机状态结构体

CutType cut_type = kCutBegin;
uint8 cut_flag=0;

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
        case kCutBegin://找角点，当有一个角点在边线45个点以内时为识别到断路，切换下一个状态
        {
            int16 corner_id_l = 0,corner_id_r = 0;
            if(CutFindCorner(&corner_id_l, &corner_id_r)!=0)
            {
                if((corner_id_l > 0.45 / SAMPLE_DIST) || (corner_id_r > 0.45 / SAMPLE_DIST))
                    break;
                else
                {
                    gpio_set_level(BEER,1);//开启蜂鸣器
                    cut_type = kCutIn;
                }

            }
            break;
        }
        case kCutIn://当角点在40个点以内时，切换成电磁循迹，到下一个状态
        {
            int16 corner_id_l = 0,corner_id_r = 0;//角点在边线第几个点
            uint8 corner_find = CutFindCorner(&corner_id_l, &corner_id_r);//找角点

            if(corner_find != 0)
            {
                CutChangeLine(corner_find, corner_id_r, corner_find);
                //切换状态，改成电磁循迹
                if(corner_id_l < 40 && corner_id_r < 40)
                {
                    cut_flag=1;//电磁偏差限幅
                    speed_type=kNormalSpeed;//关闭速度决策
                    base_speed = 60;//降速入断路
                    track_mode = kTrackADC;//切换电磁循迹
                    cut_type = kCutMid;
                    aim_distance = origin_aimdis;//恢复预瞄点
                    encoder_dis_flag = 1;
                }
            }
            else
                aim_distance = origin_aimdis;
            break;
        }
        case kCutMid://等跑到断路里面，加一点速
        {
            if(dis > 450)
            {
                encoder_dis_flag = 0;//关闭编码器积分
                cut_flag=0;//取消电磁偏差限幅
                base_speed = 62;//环内加一点速
                cut_type = kCutEndR;
            }
            break;
        }
        case kCutEndR://先判断右边线是否存在
        {
            //右边线出现，切换下一个状态
            if(r_line_count > 80 && right_line[r_line_count - 1].Y < 70)
                cut_type = kCutEndL;//切换下一个状态
            break;
        }
        case kCutEndL://先判断右边线是否存在
        {
            //边线重新出现，断路状态结束，切换成图像循迹
            if (l_line_count > 80 && left_line[l_line_count - 1].Y < 70)
            {
                speed_type=kImageSpeed;//开启速度决策
                track_mode = kTrackImage;//切换图像循迹
                cut_type = kCutBegin;//复位状态机
                gpio_set_level(BEER,0);//关闭蜂鸣器
                base_speed = original_speed;//恢复速度
                now_flag = 0;
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
* @date  : 2023.7.7
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
/***********************************************
* @brief : 断路切换左右边线判断
* @param : corner_id_l:左角点的位置
*          corner_id_r:右角点的位置
*          corner_find:找到的是哪个角点
* @return: void
* @date  : 2023.7.7
* @author: L
************************************************/
void CutChangeLine(int16 corner_id_l,int16 corner_id_r,uint8 corner_find)
{
    //有右角点，没有左角点
    if(corner_find == 2)
    {
        per_r_line_count = (int)corner_id_r;//修改边线长度到角点位置
        track_rightline(f_right_line1, per_r_line_count , center_line_r, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
        track_type = kTrackRight;
        aim_distance = (float)(corner_id_r/2)*SAMPLE_DIST;
    }
    //有左角点，没有右角点
    else if(corner_find == 1)
    {
        per_l_line_count = (int)corner_id_l;//修改边线长度到角点位置
        track_leftline(f_left_line1, per_l_line_count, center_line_l, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
        track_type = kTrackLeft;
        aim_distance = (float)(corner_id_l/2)*SAMPLE_DIST;
    }
    //两边都有角点，哪边边线长就寻那条边线
    else if(corner_find == 3)
    {
        per_l_line_count = (int)corner_id_l;//修改边线长度到角点位置
        track_leftline(f_left_line1, per_l_line_count, center_line_l, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
        track_type = kTrackLeft;
        aim_distance = (float)(corner_id_l/2)*SAMPLE_DIST;
    }

    if(aim_distance > origin_aimdis) aim_distance = origin_aimdis;//如果预瞄点比原来的远，使用原来的

    //有一边的边线不正常则寻另外一条边线
    if(r_growth_direction[7] > 25 && (r_growth_direction[0] + r_growth_direction[1]) > 20)
    {
        track_type = kTrackLeft;
    }
    else if(l_growth_direction[7] > 25 && (l_growth_direction[0] + l_growth_direction[1]) > 20)
    {
        track_type = kTrackRight;
    }
}
