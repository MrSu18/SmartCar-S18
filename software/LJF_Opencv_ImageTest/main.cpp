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

inline int Limit(int x, int low, int up)//��x���������޷�
{
    return x > up ? up : x < low ? low : x;
}

int main()
{
	//��ʼ����͸��ͼ��ѹ��
    ImagePerspective_Init();
	ImageBorderInit();
    String str = format("E:\\nodeanddata\\studio\\FSL\\Complete\\S18\\data\\image\\4.16\\0.bmp");
    ScanImage(str);
    sobel(mt9v03x_image,binary_image);
    otsu_thr=otsuThreshold(binary_image[0], MT9V03X_W, MT9V03X_H);//ʹ�ô�򷨵õ���ֵ����ֵ
    for (int i = 20 ; i < 68; i++)
    {
        /******************************************������ȡ�Ҷ�ͼ***************************************/
        String str = format("E:\\nodeanddata\\studio\\FSL\\Complete\\S18\\data\\image\\4.16\\%d.bmp", i);
        ScanImage(str);
        /************************************************************************************************/

        //��ֵ��
//        ImageBinary();
//        myadaptiveThreshold(mt9v03x_image[0],binary_image[0],MT9V03X_W,MT9V03X_H,GRAY_BLOCK ,CLIP_VALUE);
        sobel(mt9v03x_image,binary_image);
        UseImageDataToUseMat();
        PrintImage(use_mat);
//        LCDDrowRow(60,0,0,255);
        ImageProcess();

//        printf("image_bias:%f,l_line_count:%d,r_line_count:%d,c_line_count:%d\r\n",image_bias,l_line_count,r_line_count,c_line_count);
        /************************************************************************************************************/

        //�����߻�����
        PrintEdgeLine(left_line,0,l_line_count,100,100,0);
        PrintEdgeLine(right_line,0,r_line_count,0,100,100);
        PrintEdgeLine(f_left_line1,0,per_l_line_count,0,255,0);
        PrintEdgeLine(f_right_line1,0,per_r_line_count,0,255,255);
//        PrintEdgeLine(center_line_l,0,per_l_line_count,0,0,255);//��߸��ٳ���������
//        PrintEdgeLine(center_line_r,0,per_r_line_count,255,100,0);//ѭ��������
        PrintEdgeLine(center_line,0,Limit(round(aim_distance / SAMPLE_DIST), 0, c_line_count),255,0,0);//ѭ��������
        printf("%f,%d\r\n",image_bias,base_speed);
        //��ʾͼ��
        imshow("use_img", use_mat);
        waitKey(0);//�ȴ����̰���

        TrackBasicClear();

    }
    return 0;
}
