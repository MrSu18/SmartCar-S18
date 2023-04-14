#include<opencv2/opencv.hpp>
#include<iostream>
#include <string>
#include "main.h"
#include "ImageConversion.h"
#include "ImageBasic.h"
#include"ImageWR.h"
#include <cstring>
#include "ImageTrack.h"
#include "math.h"
#include "ImageProcess.h"

using namespace cv;

uint8 mt9v03x_image[MT9V03X_H][MT9V03X_W];

inline int Limit(int x, int low, int up)//给x设置上下限幅
{
    return x > up ? up : x < low ? low : x;
}

int main()
{
	//初始化逆透视图像并压缩
    ImagePerspective_Init();
	ImageBorderInit();
    for (int i = 6 ; i < 68; i++)
    {
        /******************************************总钻风获取灰度图***************************************/
        String str = format("E:\\nodeanddata\\studio\\FSL\\Complete\\S18\\data\\image\\4.13\\%d.bmp", i);
        ScanImage(str);
        /************************************************************************************************/

        //二值化
//        ImageBinary();
        myadaptiveThreshold(mt9v03x_image[0],binary_image[0],MT9V03X_W,MT9V03X_H,7 ,2);
//        sobel(mt9v03x_image,binary_image);
        UseImageDataToUseMat();
        PrintImage(use_mat);
//        LCDDrowRow(45,0,0,255);
        ImageProcess();

//        printf("image_bias:%f,l_line_count:%d,r_line_count:%d,c_line_count:%d\r\n",image_bias,l_line_count,r_line_count,c_line_count);
        /************************************************************************************************************/

        //把三线画出来
        PrintEdgeLine(f_left_line1,0,per_l_line_count,0,255,0);
        PrintEdgeLine(f_right_line1,0,per_r_line_count,0,255,255);
//        PrintEdgeLine(center_line_l,0,per_l_line_count,0,0,255);//左边跟踪出来的中线
//        PrintEdgeLine(center_line_r,0,per_r_line_count,255,100,0);//循迹的中线
        PrintEdgeLine(center_line,0,Limit(round(aim_distance / SAMPLE_DIST), 0, c_line_count),255,0,0);//循迹的中线
//        printf("%f\r\n",image_bias);
        //显示图像
        imshow("use_img", use_mat);
        waitKey(0);//等待键盘按下

        TrackBasicClear();

    }
    return 0;
}
