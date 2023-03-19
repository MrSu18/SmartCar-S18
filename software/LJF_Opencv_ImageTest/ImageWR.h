#ifndef _IMAGE_WR_H
#define _IMAGE_WR_H

#include "main.h"
#include<opencv2/opencv.hpp>
#include "ImageBasic.h"

extern cv::Mat use_mat;

void PrintImage(cv::Mat img);
void ScanImage(cv::String str);//获取图像，模拟总钻风摄像头采集图片
void UseImageDataToUseMat();//把透视矩阵的值给mat变量直接在这样方便使用opencv的函数
void LCDDrowColumn(uint8 column,uint8 R,uint8 G,uint8 B);
void LCDDrowRow(uint8 row,uint8 R,uint8 G,uint8 B);

/***********************************************
* @brief : 把边线画在图上的函数模板
* @param : line:边线数组
 *         star:开始点
 *         end:结束点
 *         RGB：颜色
* @return: 无
* @date  : 2022.12.31
* @author: 刘骏帆
************************************************/
template<typename T>//声明一个函数数据类型模版
void PrintEdgeLine(T* line,int star,int end,uint8 R,uint8 G,uint8 B)
{
    cv::Point a;
    for (uint8 i = star; i < end; i++)//bgr
    {
        a.x = line[i].X;a.y = line[i].Y;
        circle(use_mat, a, 0, cv::Scalar(B, G, R), -1); //第五个参数我设为-1，表明这是个实点。
    }
}

#endif
