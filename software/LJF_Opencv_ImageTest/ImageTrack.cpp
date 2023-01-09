//
// Created by 30516 on 2023/1/9.
//

#include "ImageTrack.h"
#include "math.h"

//============================参数================================
#define TRACK_HALF_WIDTH	23//赛道半宽像素点
//===============================================================

inline int clip(int x, int low, int up)
{
    return x > up ? up : x < low ? low : x;
}

void blur_points(myPoint* in_line, int num, myPoint* out_line, int kernel)
{
    int half = kernel / 2;
    int x=0,y=0;
    for (int i = 0; i < num; i++)
    {
        x = y = 0;
        for (int j = -half; j <= half; j++)
        {
            x += in_line[clip(i + j, 0, num - 1)].X * (half + 1 - abs(j));
            y += in_line[clip(i + j, 0, num - 1)].Y * (half + 1 - abs(j));
        }
        x /= (2 * half + 2) * (half + 1) / 2;
        y /= (2 * half + 2) * (half + 1) / 2;
        out_line[i].X=x;
        out_line[i].Y=y;
    }
}

void TrackEdgeGetCenterLine(char choose)//左右半宽巡线得到中线
{
    for (int i = 3; i < l_line_count-3; i++)
    {

        float dx = left_line[i+3].X-left_line[i-3].X;
        float dy = left_line[i+3].Y-left_line[i-3].Y;
        float dn = sqrt(dx * dx + dy * dy);
        dx /= dn;
        dy /= dn;
        center_line[i].X=left_line[i].X-dy*TRACK_HALF_WIDTH;
        center_line[i].Y=left_line[i].Y+dx*TRACK_HALF_WIDTH;
    }
}