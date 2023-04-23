//
// Created by 30516 on 2023/3/10.
//
#include "ImageSpecial.h"
#include "ImageTrack.h"
#include <stdio.h>
#include "ImageWR.h"

#define PI 3.1415926

uint8 CircleIslandLStatus()//右边环岛状态状态机
{
    static uint8 status;//环岛状态转移变量
    printf("%d,",status);
    switch (status)
    {
        case 0: //检测左环岛
            if(CircleIslandLDetection()==2)
            {
                status=1;//先默认电磁检测到就可以入环，不知道效果怎么样还没测试
            }
            break;
        case 1: //路过环岛第一个入口
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
            if (CircleIslandLOutFinish()==1)
            {
                status=5;
            }
            else CircleIslandLOut();//出环没结束就一直做出环处理
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

/***********************************************
* @brief : 检测小车是否走到了环岛元素部分
* @param : 无
* @return: 0：还没检测到左角点 1：检测到了左边角点 2：左角点消失
* @date  : 2023.3.17
* @author: 刘骏帆
* @notice: 角度./180.*PI，因为求得的曲率是弧度制的不是角度制
************************************************/
uint8 CircleIslandLDetection()//检测左环岛
{
    static uint8 status=0;//检测到角点->检测不到角点
    if (status==0)//状态0检测角点出现
    {
        for (int i = 0; i < per_l_line_count; ++i)
        {
            //检测左边70度到140度的角点,加负号的因为左边的角点算出来是负数
            //第三个条件限制的是哪个角点有多远0.8m
            if(70./180.*PI<-l_angle_1[i] && -l_angle_1[i]<140./180.*PI && i<0.8/SAMPLE_DIST)
            {
                track_type=kTrackRight;
                status=1;//第一次检测到
                break;
            }
        }
    }
    else if (status==1)//状态1检测角点消失
    {
        track_type=kTrackRight;
        if(l_line_count<2) status=2;
    }
    else if(status==2)
        status=0;//重置flag
    return status;
}

/***********************************************
* @brief : 检测是否到了需要入环的状态
* @param : 无
* @return: 是否入环 1：入环 0：入环还不用入环
* @date  : 2023.4.15
* @author: 刘骏帆
************************************************/
uint8 CircleIslandLInDetection(void)
{
    if (l_line_count>2)//先看左边是否有边线,左边有边线直接进去
    {
        track_type=kTrackLeft;
        return 1;
    }
    else
    {
        track_type=kTrackRight;//否则还没到入环时机
        return 0;
    }
}

/***********************************************
* @brief : 左环岛入环状态
* @param : 无
* @return: 是否入环结束 1：入环结束 0：入环未结束
* @date  : 2023.4.15
* @author: 刘骏帆
************************************************/
#define JUDGE_IN_EDD_THR    10//判断入环状态结束的右边线两端X坐标的差值阈值
uint8 CircleIslandLIn()//入环状态
{
    //进入此状态之后就一直开启左边线的特殊扫线（指导扫到赛道边界才开始记录数组）
    //同样的，如果第一个点的Y在图像很上方，说明这时候的车是右斜的，则进入特殊状态

    //这里是判断是否跳出入环状态
    uint8 len=0;
    if (per_r_line_count>EDGELINE_LENGTH) len=EDGELINE_LENGTH;
    else                                  len=per_r_line_count;
    if(f_right_line1[0].X-f_right_line1[len-1].X>JUDGE_IN_EDD_THR && right_line[r_line_count-1].X<80)
    {
        return 1;//入环结束
    }
    //对入环状态进行偏差处理
    if(l_line_count<2)//左边90行都丢线，说明只能沿着外边进去
    {
        //重新扫线
        LeftLineDetectionAgain();
        //对重新扫出来的线的两端来对车体位置进行判断,然后对中线进行处理
        if(left_line[0].X<left_line[l_line_count-1].X)//左边看不到圆环内环，只能看到圆环外环
        {
            uint8 y=0;//
            r_line_count=1;
            for (uint8 i=0;i<l_line_count;i++)
            {
                if(left_line[l_line_count-1-i].Y>=y)
                {
                    y=left_line[l_line_count-1-i].Y;
                }
                else if (left_line[l_line_count-1-i].Y>60)//因为灰度扫线在远处会乱打转把线搞乱，所以这里先临时这么用，后续要修改
                {
                    right_line[r_line_count]=left_line[l_line_count-i];
                    r_line_count++;
                }
            }

//            l_line_count=per_l_line_count=0;
            per_r_line_count=PER_EDGELINE_LENGTH;
            //对边线进行透视
            EdgeLinePerspective(right_line,r_line_count,per_right_line);
            BlurPoints(per_right_line, r_line_count, f_right_line, LINE_BLUR_KERNEL);
            ResamplePoints(f_right_line, r_line_count, f_right_line1, &per_r_line_count, SAMPLE_DIST*PIXEL_PER_METER);
            track_rightline(f_right_line1, per_r_line_count, center_line_r, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
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
* @date  : 2023.4.15
* @author: 刘骏帆
************************************************/
uint8 CircleIslandLOutDetection()//左环岛出环状态
{
    if (r_line_count<2)//右边一开始就丢线
    {
        //重新扫线
        RightLineDetectionAgain();
        per_r_line_count=PER_EDGELINE_LENGTH;
        EdgeLinePerspective(right_line,r_line_count,per_right_line);
        BlurPoints(per_right_line, r_line_count, f_right_line, LINE_BLUR_KERNEL);
        ResamplePoints(f_right_line, r_line_count, f_right_line1, &per_r_line_count, SAMPLE_DIST*PIXEL_PER_METER);
        local_angle_points(f_right_line1,per_r_line_count,r_angle,ANGLE_DIST/SAMPLE_DIST);
        nms_angle(r_angle,per_r_line_count,r_angle_1,(ANGLE_DIST/SAMPLE_DIST)*2+1);
        track_rightline(f_right_line1, per_r_line_count, center_line_r, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
    }
    //进行角点判断
    for (int i = 0; i < per_r_line_count; ++i)
    {
        if (70./180.*PI<r_angle_1[i] && r_angle_1[i]<140./180.*PI && i<0.8/SAMPLE_DIST)//出环右边角点
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
* @date  : 2023.4.16
* @author: 刘骏帆
************************************************/
#define TRACK_RIGHTLINE_OUT_THR  (aim_distance/SAMPLE_DIST+5)  //寻左边线出环
#define OUT_LISLAND_RIGHTADC_THR    80                         //左环出环最右边电感ADC阈值
#define OUT_LISLAND_CENTREADC_THR   80                         //左环出环中间电感ADC阈值
uint8 CircleIslandLOutFinish(void)//检测环岛是否结束
{
    static uint8 status=0;
//    if(adc_value[2]>OUT_LISLAND_CENTREADC_THR)//电磁检测到直接出环结束
//    {
//        return 1;
//    }
    //图像判断
    if (status==0)
    {
        for (int i = 0; i < per_r_line_count; ++i)
        {
            if (70./180.*PI<r_angle_1[i] && r_angle_1[i]<140./180.*PI)//出环右边角点
            {
                return 0;
            }
        }
        status=1;//到这步说明没有角点了
    }
    else if (status==1)//这个状态是检测车头正对赛道边线
    {
        if(l_line_count<3 && r_line_count<3)
            status=2;
    }
    else if(status==2)//这个状态要要等待检测右边不丢线了
    {
        if (r_line_count>5)
        {
            status = 0;
            return 1;
        }
    }
    return 0;
}
void CircleIslandLOut(void)//环岛出环处理函数
{
    myPoint_f left_inflection={0},right_inflection={0};
    //出环处理
    if (l_line_count>4)//左边如果还能看到线就沿着圆出去
    {
        track_type=kTrackLeft;
    }
    else
    {
        if (r_line_count>2)
        {
            //判断角点的位置
            for (int i = 0; i < per_r_line_count; ++i)
            {
                if (70./180.*PI<r_angle_1[i] && r_angle_1[i]<140./180.*PI)//出环右边角点
                {
                    if (i>aim_distance/SAMPLE_DIST)
                    {
                        per_r_line_count=i;
                        track_type=kTrackRight;
                    }
                    else//继承上次的偏差
                    {
                        if (image_bias<5) image_bias=8;//如果上次太小就固定打死
                        track_type=kTrackSpecial;
                    }
                    break;
                }
            }
        }
        else//左右边线都丢线
        {
            r_line_count=0;//重新规划右边线
            //找到补线的左上角点
            uint8 half=GRAY_BLOCK/2;
            myPoint left_seed=left_line[l_line_count-1];//左种子
            if (left_seed.Y==0)
            {
                left_seed.Y=USE_IMAGE_H_MAX-half-1,left_seed.X=half;
            }
            int dif_gray_value;//灰度值差比和的值
            for (; left_seed.Y>USE_IMAGE_H_MIN; left_seed.Y--)
            {
                //灰度差比和=(f(x,y)-f(x,y-1))/(f(x,y)+f(x,y-1))
                dif_gray_value=100*(use_image[left_seed.Y][left_seed.X]-use_image[left_seed.Y-1][left_seed.X])
                               /(use_image[left_seed.Y][left_seed.X]+use_image[left_seed.Y-1][left_seed.X]);
                if(dif_gray_value>10) break;
            }
            myPoint right_seed;//右种子
            right_seed.Y=USE_IMAGE_H-half-1,right_seed.X=USE_IMAGE_W-half-2;//图像右下角
            right_line[r_line_count]=right_seed;r_line_count++;//顺序不能错，不然这条线的方向就反了
            right_line[r_line_count]=left_seed;r_line_count++;
            //*****************************
            per_r_line_count=PER_EDGELINE_LENGTH;
            EdgeLinePerspective(right_line,r_line_count,per_right_line);
            ResamplePoints(per_right_line, r_line_count, f_right_line1, &per_r_line_count, SAMPLE_DIST*PIXEL_PER_METER);
            track_rightline(f_right_line1, per_r_line_count, center_line_r, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
            track_type=kTrackRight;
        }
    }
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
    if(l_line_count>10)//判断
        return 1;
    //循迹
    if(per_r_line_count>aim_distance/SAMPLE_DIST)//右边不丢线才去判断两边边线的差值避免提前出环
    {
        track_type=kTrackRight;//寻右线出去即可
    }
    else//这里对偏差进行处理，避免由于出环的时候就左拐太多，使得右边线不存在而继承上一次的偏差
    {
        //重新扫线
        RightLineDetectionAgain();
        EdgeLinePerspective(right_line,r_line_count,per_right_line);
        if(per_r_line_count>aim_distance/SAMPLE_DIST)
        {
            per_r_line_count=PER_EDGELINE_LENGTH;
            BlurPoints(per_right_line, r_line_count, f_right_line, LINE_BLUR_KERNEL);
            ResamplePoints(per_right_line, r_line_count, f_right_line1, &per_r_line_count, SAMPLE_DIST*PIXEL_PER_METER);
            track_rightline(f_right_line1, r_line_count, center_line_r, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
            track_type=kTrackRight;
        }
        else
        {
            track_type=kTrackSpecial;//偏差置为0
            image_bias=-0.5;//右拐一点
        }
    }
    return 0;
}

/***********************************************
* @brief : 从丢线找到不丢线再记录边线，左边线重新扫线
* @param : 无
* @return: 无
* @date  : 2023.4.21
* @author: 刘骏帆
************************************************/
void LeftLineDetectionAgain()
{
    uint8 half=GRAY_BLOCK/2;
    myPoint left_seed=left_line[l_line_count-1];//左种子
    if (left_seed.Y<USE_IMAGE_H_MIN+half || left_seed.Y>USE_IMAGE_H-half-1 || left_seed.X<half || left_seed.X>USE_IMAGE_W-half-1)
    {
        left_seed.Y=USE_IMAGE_H_MAX-half-1;left_seed.X=half;
    }
    left_seed.X++;
    for (; left_seed.Y > 45; left_seed.Y--)
    {
        if (PointSobelTest(left_seed) == 1)
        {
            break;
        }
    }
    l_line_count=0;//用完之后就重置清除之前扫线的错误数据
    uint8 seed_grown_result=0;//种子生长的结果
    uint8 len=EDGELINE_LENGTH-(USE_IMAGE_H_MAX-left_seed.Y);//重新扫线的长度
    while(l_line_count<len)
    {
        seed_grown_result=EightAreasSeedGrownGray(&left_seed,'l',&left_seed_num);
        if(seed_grown_result==1)
        {
            left_line[l_line_count]=left_seed;l_line_count++;
        }
        else break;
    }
}

/***********************************************
* @brief : 从丢线找到不丢线再记录边线，右边线重新扫线
* @param : 无
* @return: 无
* @date  : 2023.4.21
* @author: 刘骏帆
************************************************/
void RightLineDetectionAgain()
{
    uint8 half=GRAY_BLOCK/2;
    myPoint right_seed=right_line[r_line_count-1];//右种子
    if (right_seed.Y<USE_IMAGE_H_MIN+half || right_seed.Y>USE_IMAGE_H-half-1 || right_seed.X<half || right_seed.X>USE_IMAGE_W-half-1)
    {
        right_seed.Y=USE_IMAGE_H_MAX-half-1,right_seed.X=USE_IMAGE_W-half-1;
    }
    right_seed.X--;
    for (; right_seed.Y > 45; right_seed.Y--)
    {
        if (PointSobelTest(right_seed) == 1)
        {
            break;
        }
    }
    r_line_count=0;//用完之后就重置清除之前扫线的错误数据
    uint8 seed_grown_result=0;//种子生长的结果
    uint8 len=EDGELINE_LENGTH-(USE_IMAGE_H_MAX-right_seed.Y);//重新扫线的长度
    while(r_line_count<len)
    {
        seed_grown_result=EightAreasSeedGrownGray(&right_seed,'r',&right_seed_num);
        if(seed_grown_result==1)
        {
            right_line[r_line_count]=right_seed;r_line_count++;
        }
        else break;
    }
}
