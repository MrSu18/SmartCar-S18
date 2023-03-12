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
    EdgeDetection();
    //对边线进行滤波
    BlurPoints(left_line, l_line_count, f_left_line, LINE_BLUR_KERNEL);
    BlurPoints(right_line, r_line_count, f_right_line, LINE_BLUR_KERNEL);
    //等距采样，由于我是对透视图像进行扫线所以不需要进行等距采样
//    ResamplePoints(f_left_line, l_line_count, f_left_line1, &l_count, SAMPLE_DIST*PIXEL_PER_METER);
//    ResamplePoints(f_right_line, r_line_count, f_right_line1, &r_count, SAMPLE_DIST*PIXEL_PER_METER);
    //局部曲率
    local_angle_points(f_left_line,l_line_count,l_angle,ANGLE_DIST/SAMPLE_DIST);
    local_angle_points(f_right_line,r_line_count,r_angle,ANGLE_DIST/SAMPLE_DIST);
    //曲率极大值抑制
    nms_angle(l_angle,l_line_count,l_angle_1,(ANGLE_DIST/SAMPLE_DIST)*2+1);
    nms_angle(r_angle,r_line_count,r_angle_1,(ANGLE_DIST/SAMPLE_DIST)*2+1);
    //跟踪左线
    track_leftline(f_left_line, l_line_count, center_line_l, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
    track_rightline(f_right_line, r_line_count, center_line_r, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
    CircleIslandStatus();
    // 预瞄点求偏差
    // 单侧线少，切换巡线方向  切外向圆
    if (l_line_count < r_line_count / 2 && l_line_count < 10)
    {
        image_bias=GetAnchorPointBias(0.4,r_line_count,center_line_r);
    }
    else if (l_line_count < l_line_count / 2 && r_line_count < 10)
    {
        image_bias=GetAnchorPointBias(0.4,l_line_count,center_line_l);
    }
    else if (l_line_count < 5 && r_line_count > l_line_count)
    {
        image_bias=GetAnchorPointBias(0.4,r_line_count,center_line_r);
    }
    else if (r_line_count < 5 && l_line_count > r_line_count)
    {
        image_bias=GetAnchorPointBias(0.4,l_line_count,center_line_l);
    }
    else
    {
        image_bias=GetAnchorPointBias(0.4,r_line_count,center_line_r);
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
    l_line_count=0;r_line_count=0;//边线的计数指针清零
    l_lostline_num=0;r_lostline_num=0;//丢线数清零
}
