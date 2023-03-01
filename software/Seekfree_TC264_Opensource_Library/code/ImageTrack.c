//
// Created by 30516 on 2023/1/9.
//

#include "ImageTrack.h"
#include "math.h"
#include "stdint.h"
#include "stdio.h"

//============================参数================================
#define TRACK_HALF_WIDTH	22.5//赛道半宽像素点
//===============================================================

myPoint_f center_line_l[EDGELINE_LENGTH]={0},center_line_r[EDGELINE_LENGTH]={0},center_line[EDGELINE_LENGTH]={0};//左右边线跟踪得到的赛道中线,归一化中线
uint8 cl_line_count=0,cr_line_count=0;//中线长度
int c_line_count=0;

inline int Limit(int x, int low, int up)//给x设置上下限幅
{
    return x > up ? up : x < low ? low : x;
}
inline int Min(int a,int b) {return a<b?a:b;}//求ab最小值

/***********************************************
* @brief : 对边线进行三角平滑滤波
* @param : myPoint* in_line:扫线得到的数值为整形的边线数组
 *         int num: 边线数组的长度
 *         myPoint_f* out_line: 滤波之后浮点数据精度的边线数组
 *         uint8 kernel: 滤波一维线性卷积核大小（必须为奇数）
* @return: 无
* @date  : 2023.1.10
* @author: 上交大开源
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
* @brief : 对边线进行重新等距采样
* @param : myPoint_f* in_line:三角滤波之后的边线
*          int num1:输入边线的长度
*          myPoint_f* out_line: 等距采样后的边线（边线两点之间的实际距离相等）
*          int *num2: 输出边线的长度
*          float dist: 采样的实际距离（单位cm）
* @return: 无
* @date  : 2023.1.12
* @author: 上交大开源
************************************************/
void ResamplePoints(myPoint_f* in_line, int num1, myPoint_f* out_line, int *num2, float dist)
{
    if (num1 < 0)
    {
        *num2 = 0;
        return;
    }
    out_line[0].X = in_line[0].X;
    out_line[0].Y = in_line[0].Y;
    int len = 1;
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
* @brief : 边线局部角度变化率
* @param : myPoint_f* in_line: 输入边线
*          int num: 长度
*          float angle_out[]: 局部角度变化率数组
*          int dist
* @return: 无
* @date  : 2023.1.15
* @author: 上交大开源
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

void nms_angle(float angle_in[], int num, float angle_out[], int kernel)
{
    int half = kernel / 2;
    for (int i = 0; i < num; i++)
    {
        angle_out[i] = angle_in[i];
        for (int j = -half; j <= half; j++)
        {
            //前后如果有绝对值大于这个点的，那么这个点就设置位0
            if (fabs(angle_in[Limit(i + j, 0, num - 1)]) > fabs(angle_out[i]))
            {
                angle_out[i] = 0;
                break;
            }
        }
    }
}

/***********************************************
* @brief : 左边线半宽补线得到右边线
* @param : myPoint_f* in_line: 输入的边线
*          int num: 输入边线的长度
*          myPoint_f* out_line: 输出的中线
*          int approx_num:求法线斜率使用的前后点的个数
*          float dist:赛道半宽宽度
* @return: 无
* @date  : 2023.1.15
* @author: 上交大开源
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

// 右边线跟踪中线
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
* @brief : 得到循迹预锚点的循迹偏差
* @param : float aim_distance: 预瞄距离
*          uint8 track_line_count: 跟踪线的长度
*          myPoint_f *track_line: 跟踪的边线（左中线还是右中线）
* @return: 无
* @date  : 2023.3.1
* @author: 上交大开源
************************************************/
float GetAnchorPointBias(float aim_distance,uint8 track_line_count,myPoint_f *track_line)
{
    // 车轮对应点(纯跟踪起始点)
    float cx = USE_IMAGE_W/2;
    float cy = USE_IMAGE_H;
    float pure_angle=0;

    // 找最近点(起始点中线归一化)
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
    // 中线有点，同时最近点不是最后几个点
    if (begin_id >= 0 && track_line_count - begin_id >= 3)
    {
        // 归一化中线
        track_line[begin_id].X = cx;
        track_line[begin_id].Y = cy;
        c_line_count = sizeof(center_line) / sizeof(center_line[0]);
        ResamplePoints(track_line + begin_id, track_line_count - begin_id, center_line, &c_line_count, 0.04*50);

        // 远预锚点位置
        int aim_idx = Limit(round(aim_distance / 0.04), 0, c_line_count - 1);

        // 计算远锚点偏差值
        float dx = center_line[aim_idx].X - cx;
        float dy = cy - center_line[aim_idx].Y + 0.2 * 50;
        float dn = sqrt(dx * dx + dy * dy);
        //float error = -atan2f(dx, dy) * 180 / 3.14;

        // 纯跟踪算法(只考虑远点)
        pure_angle = -atanf(50 * 2 * 0.2 * dx / dn / dn) / 3.14 * 180 / 2.4;
    }
    else
    {
        // 中线点过少(出现问题)，则不控制舵机
        c_line_count = 0;
    }
    return pure_angle;
}
