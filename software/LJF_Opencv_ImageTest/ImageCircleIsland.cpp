//
// Created by 30516 on 2023/3/10.
//
#include "ImageSpecial.h"
#include "ImageTrack.h"
#include <stdio.h>

uint8 CircleIslandRStatus()//右边环岛状态状态机
{
    static uint8 status;//环岛状态转移变量

    switch (status)
    {
        case 0: //检测环岛
            if(CircleIslandDetection()==1)
            {
                printf("CircleIslandDetection success\r\n");
                status=1;
            }
            break;
        case 1: //路过环岛第一个入口
            break;
        case 2: //进入环岛
            break;
        case 3: //出环
            break;
        default:
            break;
    }
}


uint8 CircleIslandDetection()//检测右环岛
{
    uint8 flag=0;
    for (int i = 0; i < l_line_count/2; ++i)
    {
        if(r_angle_1[i]<1.6&&r_angle_1[i]<1.8)
        {
            return 1;//右环岛入口
        }
    }
}
