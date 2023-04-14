//
// Created by 30516 on 2023/1/9.
//

#ifndef _IMAGE_TRACK_H
#define _IMAGE_TRACK_H

#include "ImageBasic.h"//mypoint结构体

//============================参数================================
#define LINE_BLUR_KERNEL    7               //边线三角滤波核的大小
#define SAMPLE_DIST         0.01            //实际采样间距(m)
#define PIXEL_PER_METER     100              //每一米有多少像素点
#define ANGLE_DIST          0.1             //算角度的时候隔多少点去进行计算(m)
#define TRACK_WIDTH         0.4             //赛道宽度(m)
#define PER_EDGELINE_LENGTH 250             //透视后边线数组长度(cm)
//===============================================================

typedef struct myPoint_f
{
    float X;
    float Y;
}myPoint_f;//浮点数精度点的结构体，防止精度丢失

typedef enum TrackType//跟踪左右线切换的枚举类型变量
{
    kTrackLeft=0,
    kTrackRight,
    kTrackSpecial,//在元素里面就有偏差的值了
}TrackType;

//图像循迹偏差
extern float image_bias;
//预瞄点
extern float aim_distance;
//变换后左右边线
extern myPoint_f per_left_line[EDGELINE_LENGTH],per_right_line[EDGELINE_LENGTH];
// 变换后左右边线+滤波
extern myPoint_f f_left_line[EDGELINE_LENGTH],f_right_line[EDGELINE_LENGTH];
// 变换后左右边线+等距采样
extern myPoint_f f_left_line1[PER_EDGELINE_LENGTH],f_right_line1[PER_EDGELINE_LENGTH];
extern int per_l_line_count,per_r_line_count;//等距采样之后的边线长度
// 左右边线局部角度变化率
extern float l_angle[PER_EDGELINE_LENGTH],r_angle[PER_EDGELINE_LENGTH];
// 左右边线局部角度变化率+非极大抑制
extern float l_angle_1[PER_EDGELINE_LENGTH],r_angle_1[PER_EDGELINE_LENGTH];//左右边线的非极大值抑制之后的角点数组
// 左/右中线
extern myPoint_f center_line_l[PER_EDGELINE_LENGTH],center_line_r[PER_EDGELINE_LENGTH];//左右边线跟踪得到的赛道中线
// 归一化中线
extern myPoint_f center_line[PER_EDGELINE_LENGTH];//归一化中线
extern int c_line_count;//归一化中线长度
//当前的巡线方向
extern enum TrackType track_type;

void BlurPoints(myPoint_f* in_line, int num, myPoint_f* out_line, uint8 kernel);
void ResamplePoints(myPoint_f* in_line, int num1, myPoint_f* out_line, int *num2, float dist);
void local_angle_points(myPoint_f* in_line, int num, float angle_out[], int dist);
void nms_angle(float angle_in[], int num, float angle_out[], int kernel);
void track_leftline(myPoint_f* in_line, int num, myPoint_f* out_line, int approx_num, float dist);
void track_rightline(myPoint_f* in_line, int num, myPoint_f* out_line, int approx_num, float dist);
float GetAnchorPointBias(float aim_distance,uint8 track_line_count,myPoint_f *track_line);//得到循迹预锚点的循迹偏差
void FillingLine(char choose, myPoint_f point1, myPoint_f point2);
void EdgeLinePerspective(myPoint* in_line,uint8 num,myPoint_f* out_line);//边线逆透视

#endif //LJF_OPENCV_IMAGETEST_IMAGETRACK_H
