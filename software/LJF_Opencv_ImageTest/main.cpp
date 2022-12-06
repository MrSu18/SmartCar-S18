#include<opencv2/opencv.hpp>
#include<iostream>
#include <string>
#include "main.h"
#include "ImageConversion.h"
#include "ImageBasic.h"
#include"ImageWR.h"

using namespace cv;

uint8 mt9v03x_image[MT9V03X_H][MT9V03X_W];

int main()
{
	//初始化逆透视图像并压缩
	ImagePerspective_Init();
	ImageBorderInit();
	BlackBorder();

	/******************************************总钻风获取灰度图***************************************/
    String str = "E:\\nodeanddata\\studio\\FSL\\Complete\\S18\\data\\image\\11.30\\3.B.bmp";
    ScanImage(str);
	/************************************************************************************************/


    //二值化
	ImageBinary();
	PrintImage(use_image, USE_IMAGE_W, USE_IMAGE_H);
	//扫线
	EdgeDetection();
	//寻黑块
	CaculateBlackBlock();
	/************************************************************************************************************/


	//将二维数组转化为图像显示
   	uchar* ptmp = NULL;//这是关键的指针！！
	Mat gray_img2 = Mat(USE_IMAGE_H, USE_IMAGE_W, CV_8UC1);//图像img2：row*col大小
	for (int i = 0; i < USE_IMAGE_H; i++)
	{
		ptmp = gray_img2.ptr<uchar>(i);//指针指向img2的第i行
		for (int j = 0; j < USE_IMAGE_W; j++)
		{
			ptmp[j] = use_image[i][j];//二维数组数据传给img2的第i行第j列
		}
	}
	Mat img2 = Mat(USE_IMAGE_H, USE_IMAGE_W, CV_8UC3);//定义个彩色的图像类，8UC3代表三通道8bit无符号
	//颜色通道转换 Gray-->BGR!!!!,否则后面会没颜色
	cvtColor(gray_img2, img2, COLOR_GRAY2BGR);
	//把三线画出来
	Point a;
	for (uint8 i = USE_IMAGE_H - 1; i > 0; i--)//bgr
	{
		a.x = left_line[i]; a.y = i;
		circle(img2, a, 0, Scalar(0, 255, 0), -1); //第五个参数我设为-1，表明这是个实点。
		a.x = right_line[i]; a.y = i;
		circle(img2, a, 0, Scalar(0, 0, 255), -1);
		a.x = center_line[i]; a.y = i;
		circle(img2, a, 0, Scalar(255, 0, 0), -1);
	}
//	for (uint8 i = USE_IMAGE_W - 1; i > 0; i--)
//	{
//		a.x = i; a.y = left_line_y[i];
//		circle(img2, a, 0, Scalar(255, 255, 0), -1); //第五个参数我设为-1，表明这是个实点。
//		a.x = i; a.y = right_line_y[i];
//		//circle(img2, a, 0, Scalar(255, 255, 0), -1);
//	}
	// 放大图像方便查看
	int down_width = USE_IMAGE_W * 5;
	int down_height = USE_IMAGE_H * 5;
	Mat resized_image;//调整大小之后的图像
	resize(img2, resized_image, Size(down_width, down_height), INTER_LINEAR);
	//显示图像
	imshow("imag", resized_image);
	waitKey(0);//等待键盘按下
	return 0;
}

void LCDDrawPoint(uint8 row, uint8 column)
{
	//将二维数组转化为图像显示
	uchar* ptmp = NULL;//这是关键的指针！！
	Mat gray_img2 = Mat(USE_IMAGE_H, USE_IMAGE_W, CV_8UC1);//图像img2：row*col大小
	for (int i = 0; i < USE_IMAGE_H; i++)
	{
		ptmp = gray_img2.ptr<uchar>(i);//指针指向img2的第i行
		for (int j = 0; j < USE_IMAGE_W; j++)
		{
			ptmp[j] = use_image[i][j];//二维数组数据传给img2的第i行第j列
		}
	}
	Mat img2 = Mat(USE_IMAGE_H, USE_IMAGE_W, CV_8UC3);//定义个彩色的图像类，8UC3代表三通道8bit无符号
	//颜色通道转换 Gray-->BGR!!!!,否则后面会没颜色
	cvtColor(gray_img2, img2, COLOR_GRAY2BGR);
	//画点
	Point a;
	a.x = column; a.y = row;
	circle(img2, a, 0, Scalar(32, 255, 0), -1); //第五个参数我设为-1，表明这是个实点。
	// 放大图像方便查看
	int down_width = USE_IMAGE_W * 5;
	int down_height = USE_IMAGE_H * 5;
	Mat resized_image;//调整大小之后的图像
	resize(img2, resized_image, Size(down_width, down_height), INTER_LINEAR);
	//显示图像
	imshow("imag", resized_image);
	waitKey(0);//等待键盘按下
}