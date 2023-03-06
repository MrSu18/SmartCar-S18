//
// Created by 30516 on 2023/1/9.
//

#include "zf_common_headfile.h"
#include "ImageTrack.h"
#include "math.h"
#include "stdint.h"
#include "stdio.h"

myPoint_f center_line_l[EDGELINE_LENGTH]={0},center_line_r[EDGELINE_LENGTH]={0},center_line[EDGELINE_LENGTH]={0};//���ұ��߸��ٵõ�����������,��һ������
uint8 cl_line_count=0,cr_line_count=0;//���Ҹ��ٳ������ߵĳ���
int c_line_count=0;//ʵ������������Ѳ�ߵ����߳���

inline int Limit(int x, int low, int up)//��x���������޷�
{
    return x > up ? up : x < low ? low : x;
}
inline int Min(int a,int b) {return a<b?a:b;}//��ab��Сֵ

/***********************************************
* @brief : �Ա��߽�������ƽ���˲�
* @param : myPoint* in_line:ɨ�ߵõ�����ֵΪ���εı�������
 *         int num: ��������ĳ���
 *         myPoint_f* out_line: �˲�֮�󸡵����ݾ��ȵı�������
 *         uint8 kernel: �˲�һά���Ծ���˴�С������Ϊ������
* @return: ��
* @date  : 2023.1.10
* @author: �Ͻ���Դ
************************************************/
void BlurPoints(myPoint* in_line, int num, myPoint_f* out_line, uint8 kernel)
{
    int half = kernel / 2;
    float x=0,y=0;
    for (int i = 0; i < num; i++)
    {
        x = y = 0;
        for (int j = -half; j <= half; j++)
        {
            x += in_line[Limit(i + j, 0, num - 1)].X * (half + 1 - abs(j));
            y += in_line[Limit(i + j, 0, num - 1)].Y * (half + 1 - abs(j));
        }
        x /= (2 * half + 2) * (half + 1) / 2;
        y /= (2 * half + 2) * (half + 1) / 2;
        out_line[i].X=x;
        out_line[i].Y=y;
    }
}

/***********************************************
* @brief : �Ա��߽������µȾ����
* @param : myPoint_f* in_line:�����˲�֮��ı���
*          int num1:������ߵĳ���
*          myPoint_f* out_line: �Ⱦ������ı��ߣ���������֮���ʵ�ʾ�����ȣ�
*          int *num2: ������ߵĳ���
*          float dist: ������ʵ�ʾ��루��λcm��
* @return: ��
* @date  : 2023.1.12
* @author: �Ͻ���Դ
************************************************/
void ResamplePoints(myPoint_f* in_line, int num1, myPoint_f* out_line, int *num2, float dist)
{
    //�����쳣���
    if (num1 < 0)
    {
        *num2 = 0;
        return;
    }
    out_line[0].X = in_line[0].X;
    out_line[0].Y = in_line[0].Y;
    int len = 1;//������ߵĳ��ȼ���ֵ
    //��ʼ�Ⱦ����
    for (int i = 0; i < num1 - 1 && len < *num2; i++)
    {
        float x0 = in_line[i].X;
        float y0 = in_line[i].Y;
        float x1 = in_line[i + 1].X;
        float y1 = in_line[i + 1].Y;

        do
        {
            float x = out_line[len - 1].X;
            float y = out_line[len - 1].Y;

            float dx0 = x0 - x;
            float dy0 = y0 - y;
            float dx1 = x1 - x;
            float dy1 = y1 - y;

            float dist0 = sqrt(dx0 * dx0 + dy0 * dy0);
            float dist1 = sqrt(dx1 * dx1 + dy1 * dy1);

            float r0 = (dist1 - dist) / (dist1 - dist0);
            float r1 = 1 - r0;

            if (r0 < 0 || r1 < 0) break;
            x0 = x0 * r0 + x1 * r1;
            y0 = y0 * r0 + y1 * r1;
            out_line[len].X = x0;
            out_line[len].Y = y0;
            len++;
        } while (len < *num2);

    }
    *num2 = len;
}

/***********************************************
* @brief : ���߾ֲ��Ƕȱ仯��
* @param : myPoint_f* in_line: �������
*          int num: ������ߵĳ���
*          float angle_out[]: �ֲ��Ƕȱ仯�����飨��λ�������ƣ�
*          int dist: �����������ʵ�����֮��ļ��������
* @return: ��
* @date  : 2023.1.15
* @author: �Ͻ���Դ
************************************************/
void local_angle_points(myPoint_f* in_line, int num, float angle_out[], int dist)
{
    for (int i = 0; i < num; i++)
    {
        if (i <= 0 || i >= num - 1)
        {
            angle_out[i] = 0;
            continue;
        }
        float dx1 = in_line[i].X - in_line[Limit(i - dist, 0, num - 1)].X;
        float dy1 = in_line[i].Y - in_line[Limit(i - dist, 0, num - 1)].Y;
        float dn1 = sqrtf(dx1 * dx1 + dy1 * dy1);
        float dx2 = in_line[Limit(i + dist, 0, num - 1)].X - in_line[i].X;
        float dy2 = in_line[Limit(i + dist, 0, num - 1)].Y - in_line[i].Y;
        float dn2 = sqrtf(dx2 * dx2 + dy2 * dy2);
        float c1 = dx1 / dn1;
        float s1 = dy1 / dn1;
        float c2 = dx2 / dn2;
        float s2 = dy2 / dn2;
        angle_out[i] = atan2f(c1 * s2 - c2 * s1, c2 * c1 + s2 * s1);
    }
}
/***********************************************
* @brief : �ǶȾֲ�����ֵ����
* @param : float angle_in[]���洢���ߵ�ÿ��ĽǶ�
*          int num�����鳤��
*          float angle_out[]������ֵ����֮�����������
*          int kernel���ֲ��󼫴�ֵ�ķ�Χ
* @return: ��
* @date  : 2023.1.15
* @author: �Ͻ���Դ
************************************************/
void nms_angle(float angle_in[], int num, float angle_out[], int kernel)
{
    int half = kernel / 2;
    for (int i = 0; i < num; i++)
    {
        angle_out[i] = angle_in[i];
        for (int j = -half; j <= half; j++)
        {
            //ǰ������о���ֵ���������ģ���ô����������λ0
            if (fabs(angle_in[Limit(i + j, 0, num - 1)]) > fabs(angle_out[i]))
            {
                angle_out[i] = 0;
                break;
            }
        }
    }
}

/***********************************************
* @brief : ����߰���ߵõ��ұ���
* @param : myPoint_f* in_line: ����ı���
*          int num: ������ߵĳ���
*          myPoint_f* out_line: ���������
*          int approx_num:����б��ʹ�õ�ǰ���ĸ���
*          float dist:���������
* @return: ��
* @date  : 2023.1.15
* @author: �Ͻ���Դ
************************************************/
void track_leftline(myPoint_f* in_line, int num, myPoint_f* out_line, int approx_num, float dist)
{
    for (int i = 0; i < num; i++)
    {
        float dx = in_line[Limit(i + approx_num, 0, num - 1)].X - in_line[Limit(i - approx_num, 0, num - 1)].X;
        float dy = in_line[Limit(i + approx_num, 0, num - 1)].Y - in_line[Limit(i - approx_num, 0, num - 1)].Y;
        float dn = sqrt(dx * dx + dy * dy);
        dx /= dn;
        dy /= dn;
        out_line[i].X = in_line[i].X - dy * dist;
        out_line[i].Y = in_line[i].Y + dx * dist;
    }
}

// �ұ��߸�������
void track_rightline(myPoint_f* in_line, int num, myPoint_f* out_line, int approx_num, float dist)
{
    for (int i = 0; i < num; i++)
    {
        float dx = in_line[Limit(i + approx_num, 0, num - 1)].X - in_line[Limit(i - approx_num, 0, num - 1)].X;
        float dy = in_line[Limit(i + approx_num, 0, num - 1)].Y - in_line[Limit(i - approx_num, 0, num - 1)].Y;
        float dn = sqrt(dx * dx + dy * dy);
        dx /= dn;
        dy /= dn;
        out_line[i].X = in_line[i].X + dy * dist;
        out_line[i].Y = in_line[i].Y - dx * dist;
    }
}

/***********************************************
* @brief : �õ�ѭ��Ԥê���ѭ��ƫ��
* @param : float aim_distance: Ԥ�����
*          uint8 track_line_count: �����ߵĳ���
*          myPoint_f *track_line: ���ٵı��ߣ������߻��������ߣ�
* @return: ��
* @date  : 2023.3.1
* @author: �Ͻ���Դ & ������
************************************************/
float GetAnchorPointBias(float aim_distance,uint8 track_line_count,myPoint_f *track_line)
{
    // ���ֶ�Ӧ��(��������ʼ��)
    float cx = USE_IMAGE_W/2;
    float cy = USE_IMAGE_H;
    float pure_angle=0;

    // �������(��ʼ�����߹�һ��)
    float min_dist = 1e10;
    int begin_id = -1;
    for (int i = 0; i < track_line_count; i++)
    {
        float dx = track_line[i].X - cx;
        float dy = track_line[i].Y - cy;
        float dist = sqrt(dx * dx + dy * dy);
        if (dist < min_dist)
        {
            min_dist = dist;
            begin_id = i;
        }
    }
    // �����е㣬ͬʱ����㲻����󼸸���
    if (begin_id >= 0 && track_line_count - begin_id >= 3)
    {
        // ��һ������
        track_line[begin_id].X = cx;
        track_line[begin_id].Y = cy;
        c_line_count = sizeof(center_line) / sizeof(center_line[0]);
        ResamplePoints(track_line + begin_id, track_line_count - begin_id, center_line, &c_line_count, 0.04*50);

        // ԶԤê��λ��
        int aim_idx = Limit(round(aim_distance / 0.04), 0, c_line_count - 1);

        for(int i = -10;i < 10;i++)
        {
            tft180_draw_point(center_line[aim_idx + i].X, center_line[aim_idx + i].Y, RGB565_GREEN);
        }
        for(int j = -10;j < 10;j++)
        {
            tft180_draw_point(center_line[aim_idx].X + j, center_line[aim_idx].Y + j, RGB565_GREEN);
        }

        // ����Զê��ƫ��ֵ
        float dx = center_line[aim_idx].X - cx;
        float dy = cy - center_line[aim_idx].Y + 0.2 * 50;
        float dn = sqrt(dx * dx + dy * dy);
        //float error = -atan2f(dx, dy) * 180 / 3.14;

        // �������㷨(ֻ����Զ��)
        pure_angle = -atanf(50 * 2 * 0.2 * dx / dn / dn) / 3.14 * 180 / 2.4;
    }
    else
    {
        // ���ߵ����(��������)���򲻿��ƶ��
        c_line_count = 0;
    }
    return pure_angle;
}
