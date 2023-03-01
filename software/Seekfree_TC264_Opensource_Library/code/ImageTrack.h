//
// Created by 30516 on 2023/1/9.
//

#ifndef _IMAGE_TRACK_H
#define _IMAGE_TRACK_H

#include "ImageBasic.h"//mypoint结构体

typedef struct myPoint_f
{
    float X;
    float Y;
}myPoint_f;//浮点数精度点的结构体，防止精度丢失

extern myPoint_f center_line_l[EDGELINE_LENGTH],center_line_r[EDGELINE_LENGTH],center_line[EDGELINE_LENGTH];//左右边线跟踪得到的赛道中线
extern uint8 cl_line_count,cr_line_count;//中线长度
extern int c_line_count;

void BlurPoints(myPoint* in_line, int num, myPoint_f* out_line, uint8 kernel);
void ResamplePoints(myPoint_f* in_line, int num1, myPoint_f* out_line, int *num2, float dist);
void local_angle_points(myPoint_f* in_line, int num, float angle_out[], int dist);
void nms_angle(float angle_in[], int num, float angle_out[], int kernel);
void track_leftline(myPoint_f* in_line, int num, myPoint_f* out_line, int approx_num, float dist);
void track_rightline(myPoint_f* in_line, int num, myPoint_f* out_line, int approx_num, float dist);
float GetAnchorPointBias(float aim_distance,uint8 track_line_count,myPoint_f *track_line);//得到循迹预锚点的循迹偏差

#endif //LJF_OPENCV_IMAGETEST_IMAGETRACK_H
