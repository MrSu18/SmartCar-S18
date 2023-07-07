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
#include "Control.h"

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
    String str = format("E:\\IDE\\AdjustParamter\\tuchuanV2.4\\tuchuanV2.4\\record\\RecordDir_230707_204159\\img0005629.png");
    ScanImage(str);
    sobel(mt9v03x_image,binary_image);
    otsu_thr=otsuThreshold(binary_image[0], MT9V03X_W, MT9V03X_H);//使用大津法得到二值化阈值
    for (int i = 5629 ; i < 20000; i++)
    {
        /******************************************总钻风获取灰度图***************************************/
        String str = format("E:\\IDE\\AdjustParamter\\tuchuanV2.4\\tuchuanV2.4\\record\\RecordDir_230707_204159\\img000%d.png", i);
        ScanImage(str);
        /************************************************************************************************/

        //二值化
//        ImageBinary();
        myadaptiveThreshold(mt9v03x_image[0],binary_image[0],MT9V03X_W,MT9V03X_H,GRAY_BLOCK ,CLIP_VALUE);
//        sobel(mt9v03x_image,binary_image);
        UseImageDataToUseMat();
        PrintImage(use_mat);
//        LCDDrowRow(60,0,0,255);
        ImageProcess();

//        printf("image_bias:%f,l_line_count:%d,r_line_count:%d,c_line_count:%d\r\n",image_bias,l_line_count,r_line_count,c_line_count);
        /************************************************************************************************************/

        //把三线画出来
        PrintEdgeLine(left_line,0,l_line_count,100,100,0);
        PrintEdgeLine(right_line,0,r_line_count,0,100,100);
//        PrintEdgeLine(f_left_line1,0,per_l_line_count,0,255,0);
//        PrintEdgeLine(f_right_line1,0,per_r_line_count,0,255,255);
//        PrintEdgeLine(center_line_l,0,per_l_line_count,0,0,255);//左边跟踪出来的中线
//        PrintEdgeLine(center_line_r,0,per_r_line_count,255,100,0);//循迹的中线
        PrintEdgeLine(center_line,0,Limit(round(aim_distance / SAMPLE_DIST), 0, c_line_count),255,0,0);//循迹的中线
//        printf("%f,%d\r\n",image_bias,base_speed);
        printf("种子生长方向:\r\n");
        printf("%d,%d,%d\r\n",r_growth_direction[1],r_growth_direction[2],r_growth_direction[3]);
        printf("%d,S,%d\r\n",r_growth_direction[0],r_growth_direction[4]);
        printf("%d,%d,%d\r\n",r_growth_direction[7],r_growth_direction[6],r_growth_direction[5]);
        //显示图像
        imshow("use_img", use_mat);
        waitKey(0);//等待键盘按下

        TrackBasicClear();

    }
    return 0;
}
