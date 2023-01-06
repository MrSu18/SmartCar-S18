#include<opencv2/opencv.hpp>
#include<iostream>
#include <string>
#include "main.h"
#include "ImageConversion.h"
#include "ImageBasic.h"
#include"ImageWR.h"
#include <cstring>

using namespace cv;

uint8 mt9v03x_image[MT9V03X_H][MT9V03X_W];

int main()
{
	//初始化逆透视图像并压缩
	ImagePerspective_Init();
	ImageBorderInit();
	BlackBorder();
    for (int i = 1; i < 42; i++)
    {
        /******************************************总钻风获取灰度图***************************************/
        String str = format("E:\\nodeanddata\\studio\\FSL\\Complete\\S18\\data\\image\\11.30\\%d.bmp", i);
        //String str = "E:\\nodeanddata\\studio\\FSL\\Complete\\S18\\data\\image\\11.30\\10.bmp";
        ScanImage(str);
        /************************************************************************************************/

        //二值化
        ImageBinary();
        UseImageDataToUseMat();
        PrintImage(use_mat);
        //扫线
        EdgeDetection();
        //寻黑块
        CaculateBlackBlock();
        /************************************************************************************************************/

        //把三线画出来
//        Point a;
//        for (uint8 i = USE_IMAGE_H - 1; i > 0; i--)//bgr
//        {
//            a.x = left_line_x[i];
//            a.y = i;
//            circle(use_mat, a, 0, Scalar(0, 255, 0), -1); //第五个参数我设为-1，表明这是个实点。
//            a.x = right_line_x[i];
//            a.y = i;
//            circle(use_mat, a, 0, Scalar(0, 0, 255), -1);
//            a.x = center_line_x[i];
//            a.y = i;
//            circle(use_mat, a, 0, Scalar(255, 0, 0), -1);
//        }
        //数组清零
//        memcpy(left_line_x,left_border,sizeof(uint8)*USE_IMAGE_H);
//        memcpy(right_line_x,right_border,sizeof(uint8)*USE_IMAGE_H);
//        memset(center_line_x,USE_IMAGE_W/2,sizeof(uint8)*USE_IMAGE_H);
        //显示图像
        imshow("use_img", use_mat);
        waitKey(0);//等待键盘按下
    }
    return 0;
}

void LCDDrawPoint(uint8 row, uint8 column)
{
    //画点
    Point a;
    a.x = column; a.y = row;
    circle(use_mat, a, 0, Scalar(32, 255, 0), -1); //第五个参数我设为-1，表明这是个实点。
	//显示图像
	imshow("use_img", use_mat);
	waitKey(0);//等待键盘按下
}