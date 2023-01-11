//
// Created by 30516 on 2023/1/9.
//

#ifndef LJF_OPENCV_IMAGETEST_IMAGETRACK_H
#define LJF_OPENCV_IMAGETEST_IMAGETRACK_H

#include "ImageBasic.h"//mypoint结构体

typedef struct myPoint_f
{
    float X;
    float Y;
}myPoint_f;//浮点数精度点的结构体，防止精度丢失

extern myPoint_f center_line[EDGELINE_LENGTH];//赛道中线
extern uint8 c_line_count;//中线长度

void BlurPoints(myPoint* in_line, int num, myPoint_f* out_line, uint8 kernel);
void resample_points(myPoint_f* in_line, int num1, myPoint_f* out_line, int *num2, float dist);
void local_angle_points(myPoint_f* in_line, int num, float angle_out[], int dist);
void nms_angle(float angle_in[], int num, float angle_out[], int kernel);
void track_leftline(myPoint_f* in_line, int num, myPoint_f* out_line, int approx_num, float dist);

#endif //LJF_OPENCV_IMAGETEST_IMAGETRACK_H
