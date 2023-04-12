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
* @param : cv::Mat img：要显示的mat类
* @return: 显示图像
* @date  : 2022.12.31
* @author: 刘骏帆
************************************************/
void PrintImage(cv::Mat img)
{
	//显示图像
    namedWindow("use_img",WINDOW_NORMAL);//设置imag窗口调整窗口为可以无限制调整窗口大小，否则使用imshow默认的参数是WINDOW_AUTOSIZE是只能固定大小
	imshow("use_img", img);
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
    Mat gray_img = imread(str,IMREAD_GRAYSCALE);//以灰度图的形式读取图像
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
    namedWindow("原始灰度图",WINDOW_NORMAL);
    imshow("原始灰度图",gray_img);
}

/***********************************************
* @brief : 创建个mat值是透视矩阵的值
* @param : 无
* @return: 无
* @date  : 2022.12.31
* @author: 刘骏帆
************************************************/
Mat use_mat;//做图像处理中使用的二维指针变量数组的数据
void UseImageDataToUseMat()
{
    Mat img=Mat(USE_IMAGE_H, USE_IMAGE_W, CV_8UC1);//临时的变量，用于接收图像数据
    use_mat=Mat(USE_IMAGE_H, USE_IMAGE_W, CV_8UC3);//给全局变量mat进行实例化

    for (int i = 0; i < USE_IMAGE_H; i++)
    {
        for (int j = 0; j < USE_IMAGE_W; j++)
        {
            img.at<uchar>(i, j)=*PerImg_ip[i][j];//二维数组数据传给img2的第i行第j列
        }
    }
    //颜色通道转换 Gray-->BGR!!!!,否则后面会没颜色
    cvtColor(img, use_mat, COLOR_GRAY2BGR);
}

void LCDDrawPoint(uint8 row, uint8 column,uint8 R,uint8 G,uint8 B)
{
    //画点
    Point a;
    a.x = column; a.y = row;
    circle(use_mat, a, 0, Scalar(B, G, R), -1); //第五个参数我设为-1，表明这是个实点。
    //显示图像
    imshow("use_img", use_mat);
    waitKey(0);//等待键盘按下
}

void LCDDrowColumn(uint8 column,uint8 R,uint8 G,uint8 B)
{
    for (int i = 0; i < USE_IMAGE_H; ++i)
    {
        LCDDrawPoint(i,column,R,G,B);
    }
}

void LCDDrowRow(uint8 row,uint8 R,uint8 G,uint8 B)
{
    for (int i = 0; i < USE_IMAGE_W; ++i)
    {
        LCDDrawPoint(row,i,R,G,B);
    }
}
