#include "main.h"
#include<opencv2/opencv.hpp>

void PrintImage(uint8* image[], uint16 width, uint16 height);//将一个二维数组显示图像
void ScanImage(cv::String str);//获取图像，模拟总钻风摄像头采集图片
