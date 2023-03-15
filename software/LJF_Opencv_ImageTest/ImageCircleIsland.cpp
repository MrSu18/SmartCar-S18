//
// Created by 30516 on 2023/3/10.
//
#include "ImageSpecial.h"
#include "ImageTrack.h"
#include <stdio.h>
#include "ImageWR.h"

uint8 CircleIslandLStatus()//右边环岛状态状态机
{
    static uint8 status;//环岛状态转移变量

    switch (status)
    {
        case 0: //检测左环岛
            if(CircleIslandLDetection()==1)
            {
                printf("CircleIslandDetection success\r\n");
                status=1;
                track_type=kTrackRight;
            }
            break;
        case 1: //路过环岛第一个入口
            track_type=kTrackRight;//这个状态默认是寻右边直线
            if(1)
            {
                status=2;
                track_type=kTrackLeft;
            }
            break;
        case 2: //进入环岛
            break;
        case 3: //出环
            break;
        default:
            break;
    }
    return 0;
}


uint8 CircleIslandLDetection()//检测左环岛
{
    //用电磁检测环岛
    uint8 flag=0;
    for (int i = 0; i < l_line_count/2; ++i)
    {
        if(r_angle_1[i]<1.6&&r_angle_1[i]<1.8)
        {
            return 1;//右环岛入口
        }
    }
    return 0;
}

uint8 CircleIslandLInDetection()//检测是否到了需要入环的状态
{
    //进入此状态之后就一直开启左边线的特殊扫线（指导扫到赛道边界才开始记录数组）
    //通过判断第一个点的Y坐标来判断车的位置，当低于阈值（H-4），则可以准备入环
    //同样的，如果第一个点的Y在图像很上方，说明这时候的车是右斜的，则进入特殊状态

    if(l_line_count<5)
    {
        //进行重新扫线
        myPoint left_seed=left_line[l_line_count-1];//左种子
        l_line_count=0;//用完之后就重置清除之前扫线的错误数据
        uint8 left_seed_num=0;//左种子八零域标号
        uint8 seed_grown_result=0;//种子生长的结果
        uint8 flag=0;//从丢线到不丢线,0:还没找到过边界，1:已经找到边界
        while(l_line_count<EDGELINE_LENGTH)
        {
            seed_grown_result=EightAreasSeedGrown(&left_seed,'l',&left_seed_num);
            if(seed_grown_result==1)
            {
                flag=1;
                left_line[l_line_count]=left_seed;l_line_count++;
            }
            else if(seed_grown_result==2)
            {
                if(flag==0) continue;
                else        break;
            }
            else break;
        }
        //对重新扫出来的线的两端来对车体位置进行判断,然后对中线进行处理
        if (left_line[0].Y>USE_IMAGE_H*2/3 && left_line[0].X>left_line[l_line_count-1].X)//正常的再次扫线扫到了圆环内环，沿这条线进去
        {
            //对边线进行滤波
            BlurPoints(left_line, l_line_count, f_left_line, LINE_BLUR_KERNEL);
            //跟踪左线
            track_leftline(f_left_line, l_line_count, center_line_l, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
            track_type=kTrackLeft;
        }
        else if(left_line[0].X<left_line[l_line_count-1].X)//左边看不到圆环内环，只能看到圆环外环
        {
            r_line_count=0;
            for (uint8 i=0;i<l_line_count;i++)
            {
                right_line[r_line_count]=left_line[l_line_count-i];
                r_line_count++;
            }
            l_line_count=0;
            BlurPoints(right_line, r_line_count, f_right_line, LINE_BLUR_KERNEL);
            track_rightline(f_right_line, r_line_count, center_line_r, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
            track_type=kTrackRight;
        }
    }
    else
    {
        track_type=kTrackLeft;//如果没丢线寻左边线的圆环进去
    }
    return 0;
}
