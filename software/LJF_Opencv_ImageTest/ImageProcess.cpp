//
// Created by 30516 on 2023/3/6.
//
#include "ImageProcess.h"
#include "ImageConversion.h"
#include "ImageBasic.h"
#include "ImageTrack.h"
#include "math.h"
#include <string.h>
#include "ImageSpecial.h"
#include "ImageWR.h"

/***********************************************
* @brief : 图像处理和元素识别
* @param : 无
* @return: 无
* @date  : 2023.3.6
* @author: 刘骏帆
************************************************/
void ImageProcess(void)
{
    //扫线
    myPoint left_seed,right_seed;
    SowSeedGray(2, 10, &left_seed, &right_seed);
    Findline_Lefthand_Adaptive(5,2,left_seed,left_line,&l_line_count);
    Findline_Righthand_Adaptive(5, 2, right_seed, right_line, &r_line_count);
    //对边线进行滤波
//    BlurPoints(left_line, l_line_count, f_left_line, LINE_BLUR_KERNEL);
//    BlurPoints(right_line, r_line_count, f_right_line, LINE_BLUR_KERNEL);
    //等距采样，由于我是对透视图像进行扫线所以不需要进行等距采样
//    ResamplePoints(f_left_line, l_line_count, f_left_line1, &l_count, SAMPLE_DIST*PIXEL_PER_METER);
//    ResamplePoints(f_right_line, r_line_count, f_right_line1, &r_count, SAMPLE_DIST*PIXEL_PER_METER);
    //局部曲率
//    local_angle_points(f_left_line,l_line_count,l_angle,ANGLE_DIST/SAMPLE_DIST);
//    local_angle_points(f_right_line,r_line_count,r_angle,ANGLE_DIST/SAMPLE_DIST);
    //曲率极大值抑制
//    nms_angle(l_angle,l_line_count,l_angle_1,(ANGLE_DIST/SAMPLE_DIST)*2+1);
//    nms_angle(r_angle,r_line_count,r_angle_1,(ANGLE_DIST/SAMPLE_DIST)*2+1);
    //跟踪左线
//    track_leftline(f_left_line, l_line_count, center_line_l, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
//    track_rightline(f_right_line, r_line_count, center_line_r, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
//    LCDDrowColumn(f_right_line[19].X,100,100,0);
//    LCDDrowRow(f_right_line[19].Y,100,100,0);
    //切换左右巡线
    if(r_line_count>2 && l_line_count < 20)
    {
        track_type=kTrackRight;
    }
    else if(l_line_count>2 && r_line_count < 20)
    {
        track_type=kTrackLeft;
    }
    else if(r_line_count>2 && l_line_count<2)
    {
        track_type=kTrackRight;
    }
    else if(l_line_count>2 && r_line_count<2)
    {
        track_type=kTrackLeft;
    }
//    if(CircleIslandLStatus()==1)
//        while (1);
    //预瞄点求偏差
    if(track_type==kTrackRight)
    {
        image_bias = GetAnchorPointBias(aim_distance, r_line_count, center_line_r);
    }
    else if(track_type==kTrackLeft)
    {
        image_bias = GetAnchorPointBias(aim_distance, l_line_count, center_line_l);
    }
}

/***********************************************
* @brief : 赛道基础信息变量重置，为下一帧做准备
* @param : 全局变量
* @return: 初始化为0的全局变量
* @date  : 2023.3.6
* @author: 刘骏帆
* @note	 :  无
************************************************/
void TrackBasicClear(void)
{
    //边线丢线数组清零
    memset(l_lost_line,LOST_LINE_FALSE,sizeof(char)*EDGELINE_LENGTH);
    memset(r_lost_line,LOST_LINE_FALSE,sizeof(char)*EDGELINE_LENGTH);
    l_line_count=EDGELINE_LENGTH;r_line_count=EDGELINE_LENGTH;//边线的计数指针清零
    l_lostline_num=0;r_lostline_num=0;//丢线数清零
}
