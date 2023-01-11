//
// Created by 30516 on 2023/1/9.
//

#include "ImageTrack.h"
#include "math.h"
#include "stdint.h"

//============================参数================================
#define TRACK_HALF_WIDTH	22.5//赛道半宽像素点
//===============================================================

myPoint_f center_line[EDGELINE_LENGTH];//赛道中线
uint8 c_line_count=0;//中线长度

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
* @author: 刘骏帆
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

void resample_points(myPoint_f* in_line, int num1, myPoint_f* out_line, int *num2, float dist)
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
            if (fabs(angle_in[Limit(i + j, 0, num - 1)]) > fabs(angle_out[i]))
            {
                angle_out[i] = 0;
                break;
            }
        }
    }
}

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
