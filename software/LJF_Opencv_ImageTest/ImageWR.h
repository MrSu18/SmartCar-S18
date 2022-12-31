#include "main.h"
#include<opencv2/opencv.hpp>

extern cv::Mat use_mat;

void PrintImage(cv::Mat img);
void ScanImage(cv::String str);//获取图像，模拟总钻风摄像头采集图片
void UseImageDataToUseMat();//把透视矩阵的值给mat变量直接在这样方便使用opencv的函数
