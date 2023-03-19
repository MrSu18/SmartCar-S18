#ifndef _IMAGE_WR_H
#define _IMAGE_WR_H

#include "main.h"
#include<opencv2/opencv.hpp>
#include "ImageBasic.h"

extern cv::Mat use_mat;

void PrintImage(cv::Mat img);
void ScanImage(cv::String str);//��ȡͼ��ģ�����������ͷ�ɼ�ͼƬ
void UseImageDataToUseMat();//��͸�Ӿ����ֵ��mat����ֱ������������ʹ��opencv�ĺ���
void LCDDrowColumn(uint8 column,uint8 R,uint8 G,uint8 B);
void LCDDrowRow(uint8 row,uint8 R,uint8 G,uint8 B);

/***********************************************
* @brief : �ѱ��߻���ͼ�ϵĺ���ģ��
* @param : line:��������
 *         star:��ʼ��
 *         end:������
 *         RGB����ɫ
* @return: ��
* @date  : 2022.12.31
* @author: ������
************************************************/
template<typename T>//����һ��������������ģ��
void PrintEdgeLine(T* line,int star,int end,uint8 R,uint8 G,uint8 B)
{
    cv::Point a;
    for (uint8 i = star; i < end; i++)//bgr
    {
        a.x = line[i].X;a.y = line[i].Y;
        circle(use_mat, a, 0, cv::Scalar(B, G, R), -1); //�������������Ϊ-1���������Ǹ�ʵ�㡣
    }
}

#endif
