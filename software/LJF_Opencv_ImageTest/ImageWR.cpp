/*===========================================
== 封装OpenCV函数用于读写图像便于DEBUG
== 作者：刘骏帆
=============================================*/

#include<opencv2/opencv.hpp>
#include "ImageWR.h"
#include "ImageConversion.h"
using namespace cv;

/***********************************************
* @brief : 将二维数组作为图片显示
* @param : uint8* image[]:存储图像行地址的一维指针数组（传二维数组元素为指针变量）
*		   uint16 width：宽度
*		   uint16 height：高度
* @return: 显示图像
* @date  : 2022.8.29
* @author: 刘骏帆
************************************************/
void PrintImage(uint8* image[], uint16 width, uint16 height)//将一个二维数组显示图像
{
	//将二维数组转化为图像显示
	uchar* ptmp = NULL;//这是关键的指针！！
	Mat img = Mat(height, width, CV_8UC1);//图像img2：row*col大小
	for (int i = 0; i < height; i++)
	{
		ptmp = img.ptr<uchar>(i);//指针指向img2的第i行
		for (int j = 0; j < width; j++)
		{
			ptmp[j] = *(image[i * width + j]);//二维数组数据传给img2的第i行第j列
			//ptmp[j] = binary_image[i][j];//二维数组数据传给img2的第i行第j列
		}
	}
	// 放大图像方便查看
	int down_width = USE_IMAGE_W * 5;
	int down_height = USE_IMAGE_H * 5;
	Mat resized_image;//调整大小之后的图像
	resize(img, resized_image, Size(down_width, down_height), INTER_LINEAR);
	//显示图像
	imshow("imag", resized_image);
	waitKey(0);//等待键盘按下
}

/***********************************************
* @brief : 获取图像，模拟总钻风摄像头采集图片
* @param : String str：图片的绝对路径
* @return: 显示图像
* @date  : 2022.12.2
* @author: 刘骏帆
************************************************/
void ScanImage(cv::String str)
{
    //获取图像
    Mat img = imread(str);
    Mat gray_img;
    //颜色通道转换 BGR-->Gray
    cvtColor(img, gray_img, COLOR_BGR2GRAY);//无论你都进来的是不是都要转为单通道的图才行，否则就算是读bmp也是三通道
    //将图像转化为二维数组
    int i = 0, j = 0;
    int row = gray_img.rows;
    int col = gray_img.cols;
    for (i = 0; i < row; i++)
    {
        for (j = 0; j < col; j++)
        {
            mt9v03x_image[i][j] = gray_img.at<uchar>(i, j);//img的矩阵数据传给二维数组ptr[][]
        }
    }
    imshow("原始灰度图",img);
}
