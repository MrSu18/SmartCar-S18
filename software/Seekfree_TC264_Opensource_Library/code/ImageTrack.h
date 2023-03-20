//
// Created by 30516 on 2023/1/9.
//

#ifndef _IMAGE_TRACK_H
#define _IMAGE_TRACK_H

#include "ImageBasic.h"//mypoint�ṹ��

//============================����================================
#define LINE_BLUR_KERNEL    3               //���������˲��˵Ĵ�С
#define SAMPLE_DIST         0.02            //ʵ�ʲ������(m)
#define PIXEL_PER_METER     50              //ÿһ���ж������ص�
#define ANGLE_DIST          0.1             //��Ƕȵ�ʱ������ٵ�ȥ���м���(m)
#define TRACK_WIDTH         0.4             //�������(m)
//===============================================================

typedef struct myPoint_f
{
    float X;
    float Y;
}myPoint_f;//���������ȵ�Ľṹ�壬��ֹ���ȶ�ʧ

typedef enum TrackType//�����������л���ö�����ͱ���
{
    kTrackLeft=0,
    kTrackRight,
}TrackType;

//ͼ��ѭ��ƫ��
extern float image_bias;
//Ԥ���
extern float aim_distance;
// �任�����ұ���+�˲�
extern myPoint_f f_left_line[EDGELINE_LENGTH],f_right_line[EDGELINE_LENGTH];
// �任�����ұ���+�Ⱦ����
extern myPoint_f f_left_line1[EDGELINE_LENGTH],f_right_line1[EDGELINE_LENGTH];
// ���ұ��߾ֲ��Ƕȱ仯��
extern float l_angle[EDGELINE_LENGTH],r_angle[EDGELINE_LENGTH];
// ���ұ��߾ֲ��Ƕȱ仯��+�Ǽ�������
extern float l_angle_1[EDGELINE_LENGTH],r_angle_1[EDGELINE_LENGTH];//���ұ��ߵķǼ���ֵ����֮��Ľǵ�����
// ��/������
extern myPoint_f center_line_l[EDGELINE_LENGTH],center_line_r[EDGELINE_LENGTH];//���ұ��߸��ٵõ�����������
// ��һ������
extern myPoint_f center_line[EDGELINE_LENGTH];//��һ������
extern int c_line_count;//��һ�����߳���
//��ǰ��Ѳ�߷���
extern enum TrackType track_type;

void BlurPoints(myPoint* in_line, int num, myPoint_f* out_line, uint8 kernel);
void ResamplePoints(myPoint_f* in_line, int num1, myPoint_f* out_line, int *num2, float dist);
void local_angle_points(myPoint_f* in_line, int num, float angle_out[], int dist);
void nms_angle(float angle_in[], int num, float angle_out[], int kernel);
void track_leftline(myPoint_f* in_line, int num, myPoint_f* out_line, int approx_num, float dist);
void track_rightline(myPoint_f* in_line, int num, myPoint_f* out_line, int approx_num, float dist);
float GetAnchorPointBias(float aim_distance,uint8 track_line_count,myPoint_f *track_line);//�õ�ѭ��Ԥê���ѭ��ƫ��
void FillingLine(char choose, myPoint_f point1, myPoint_f point2);

#endif //LJF_OPENCV_IMAGETEST_IMAGETRACK_H
