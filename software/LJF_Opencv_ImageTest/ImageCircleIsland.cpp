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
                status=1;//先默认电磁检测到就可以入环，不知道效果怎么样还没测试
                track_type=kTrackRight;
            }
            break;
        case 1: //路过环岛第一个入口
            track_type=kTrackRight;//这个状态默认是寻右边直线
            if(CircleIslandLInDetection()==1)
            {
                status=2;
            }
            break;
        case 2: //进入环岛
            if(CircleIslandLIn()==1)
            {
                status=3;
            }
            break;
        case 3: //检测出环
            if(CircleIslandLOutDetection()==1)
            {
                track_type=kTrackRight;
                status=4;
            }
            break;
        case 4://出环
            if (CircleIslandLOut()==1)
            {
                status=5;
            }
            break;
        case 5://检测环岛是否结束
            if (CircleIslandLEnd()==1)
            {
                status=0;
                return 1;
            }
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

/***********************************************
* @brief : 检测是否到了需要入环的状态
* @param : 无
* @return: 是否入环 1：入环 0：入环还不用入环
* @date  : 2023.3.17
* @author: 刘骏帆
************************************************/
uint8 CircleIslandLInDetection(void)
{
    if(l_line_count<2)//如果左边一开始就丢线就再次扫线
    {
        myPoint left_seed=left_line[l_line_count-1];//左种子
//        uint8 left_seed_num=0;//左种子八零域标号
        uint8 seed_grown_result=0;//种子生长的结果
        uint8 count=0;//记录左边丢线多少才到不丢线
        while(count<3)
        {
            seed_grown_result=EightAreasSeedGrown(&left_seed,'l',&left_seed_num);
            if(seed_grown_result==1)
            {
                return 1;
            }
            else if(seed_grown_result==2)
            {
                count++;
                continue;
            }
            else break;
        }
        return 0;
    }
    return 0;
}

/***********************************************
* @brief : 左环岛入环状态
* @param : 无
* @return: 是否入环结束 1：入环结束 0：入环未结束
* @date  : 2023.3.17
* @author: 刘骏帆
************************************************/
#define JUDGE_IN_EDD_THR    10//判断入环状态结束的右边线两端X坐标的差值阈值
uint8 CircleIslandLIn()//入环状态
{
    //进入此状态之后就一直开启左边线的特殊扫线（指导扫到赛道边界才开始记录数组）
    //通过判断第一个点的Y坐标来判断车的位置，当低于阈值（H-4），则可以准备入环
    //同样的，如果第一个点的Y在图像很上方，说明这时候的车是右斜的，则进入特殊状态

    //这里是判断是否跳出入环状态
    if(right_line[0].X-right_line[r_line_count-1].X>JUDGE_IN_EDD_THR)
    {
        return 1;//入环结束
    }
    //对入环状态进行偏差处理
    if(l_line_count<2)
    {
        //重新扫线
        LeftLineDetectionAgain();
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
            uint8 y=0;//
            r_line_count=0;
            for (uint8 i=0;i<l_line_count;i++)
            {
                if(left_line[l_line_count-i].Y>=y)
                {
                    y=left_line[l_line_count-i].Y;
                }
                else
                {
                    right_line[r_line_count]=left_line[l_line_count-i];
                    r_line_count++;
                }
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

/***********************************************
* @brief : 左环岛检测出环
* @param : 无
* @return: 是否到出环状态 1：环岛出口 0：还在环中
* @date  : 2023.3.19
* @author: 刘骏帆
************************************************/
uint8 CircleIslandLOutDetection()//左环岛出环状态
{
    if (r_line_count<2)//右边一开始就丢线
    {
        //重新扫线
        RightLineDetectionAgain();
        BlurPoints(right_line, r_line_count, f_right_line, LINE_BLUR_KERNEL);
        local_angle_points(f_right_line,r_line_count,r_angle,ANGLE_DIST/SAMPLE_DIST);
        nms_angle(r_angle,r_line_count,r_angle_1,(ANGLE_DIST/SAMPLE_DIST)*2+1);
        track_rightline(f_right_line, r_line_count, center_line_r, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
    }
    //进行角点判断
    for (int i = 0; i < r_line_count; ++i)
    {
        if (1.5<r_angle_1[i] && r_angle_1[i]<1.8)//出环右边角点
        {
            return 1;
        }
    }
    return 0;
}

/***********************************************
* @brief : 左环岛出环
* @param : 无
* @return: 0：出环未结束 1：出环结束
* @date  : 2023.3.19
* @author: 刘骏帆
************************************************/
#define TRACK_RIGHTLINE_OUT_THR  (aim_distance/SAMPLE_DIST+5)  //寻左边线出环
uint8 CircleIslandLOut(void)
{
    static uint8 status=0;//右边线从不丢线，到丢线，再到不丢线，即环岛结束
    //对循迹偏差进行处理
    if(r_line_count>TRACK_RIGHTLINE_OUT_THR)
    {
        if (status==0)
        {
            status=1;
        }
        else if(status==2)
        {
            status=0;
            return 1;
        }
        track_type=kTrackRight;
    }
    else//如果左边线太少了需要补线出环
    {
        if (status==1)
        {
            status=2;
        }
        myPoint_f left_inflection={0},right_inflection={0};//补线的左右点
        //先找到右角点
        if (r_line_count<2)//右边一开始就丢线
        {
            //重新扫线
            RightLineDetectionAgain();
            BlurPoints(right_line, r_line_count, f_right_line, LINE_BLUR_KERNEL);
            local_angle_points(f_right_line,r_line_count,r_angle,ANGLE_DIST/SAMPLE_DIST);
            nms_angle(r_angle,r_line_count,r_angle_1,(ANGLE_DIST/SAMPLE_DIST)*2+1);
        }
        for (int i = 0; i < r_line_count; ++i)
        {
            if (1.5<r_angle_1[i] && r_angle_1[i]<1.8)//出环右边角点
            {
                r_line_count=i;//截断
                right_inflection=f_right_line[i];
                break;
            }
        }
        //找左拐点
        if (l_line_count<2 && right_inflection.Y!=0 && right_inflection.X!=0)//如果左边一开始就丢线,并且找到了右拐点
        {
            myPoint left_seed=left_line[l_line_count-1];//左种子
//            uint8 left_seed_num=0;//左种子八零域标号
            uint8 seed_grown_result=0;//种子生长的结果
            while(seed_grown_result!=1)
            {
                seed_grown_result=EightAreasSeedGrown(&left_seed,'l',&left_seed_num);
            }
            left_inflection.X=(float)left_seed.X;left_inflection.Y=(float)left_seed.Y;
            FillingLine('r',left_inflection,right_inflection);
        }
        //左右两点连线之后再对右边线进行处理
        track_rightline(f_right_line, r_line_count, center_line_r, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
        track_type=kTrackRight;
    }
    return 0;
}

/***********************************************
* @brief : 判断左环岛是否结束
* @param : 无
* @return: 无
* @date  : 2023.3.19
* @author: 刘骏帆
************************************************/
uint8 CircleIslandLEnd(void)
{
    track_type=kTrackRight;//寻右线出去即可
    if(abs(r_line_count-l_line_count)<10)
        return 1;
    else
        return 0;
}

/***********************************************
* @brief : 从丢线找到不丢线再记录边线，左边线重新扫线
* @param : 无
* @return: 无
* @date  : 2023.3.17
* @author: 刘骏帆
************************************************/
void LeftLineDetectionAgain()
{
    myPoint left_seed=left_line[l_line_count-1];//左种子
    l_line_count=0;//用完之后就重置清除之前扫线的错误数据
//    uint8 left_seed_num=0;//左种子八零域标号
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
}

/***********************************************
* @brief : 从丢线找到不丢线再记录边线，右边线重新扫线
* @param : 无
* @return: 无
* @date  : 2023.3.19
* @author: 刘骏帆
************************************************/
void RightLineDetectionAgain()
{
    myPoint right_seed=right_line[r_line_count-1];//右种子
    r_line_count=0;//用完之后就重置清除之前扫线的错误数据
//    uint8 right_seed_num=0;//左种子八零域标号
    uint8 seed_grown_result=0;//种子生长的结果
    uint8 flag=0;//从丢线到不丢线,0:还没找到过边界，1:已经找到边界
    while(r_line_count<EDGELINE_LENGTH)
    {
        seed_grown_result=EightAreasSeedGrown(&right_seed,'r',&right_seed_num);
        if(seed_grown_result==1)
        {
            flag=1;
            right_line[r_line_count]=right_seed;r_line_count++;
        }
        else if(seed_grown_result==2)
        {
            if(flag==0) continue;
            else        break;
        }
        else break;
    }
}
