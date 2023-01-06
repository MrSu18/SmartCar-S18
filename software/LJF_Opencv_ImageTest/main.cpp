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
	//��ʼ����͸��ͼ��ѹ��
	ImagePerspective_Init();
	ImageBorderInit();
	BlackBorder();
    for (int i = 1; i < 42; i++)
    {
        /******************************************������ȡ�Ҷ�ͼ***************************************/
        String str = format("E:\\nodeanddata\\studio\\FSL\\Complete\\S18\\data\\image\\11.30\\%d.bmp", i);
        //String str = "E:\\nodeanddata\\studio\\FSL\\Complete\\S18\\data\\image\\11.30\\10.bmp";
        ScanImage(str);
        /************************************************************************************************/

        //��ֵ��
        ImageBinary();
        UseImageDataToUseMat();
        PrintImage(use_mat);
        //ɨ��
        EdgeDetection();
        //Ѱ�ڿ�
        CaculateBlackBlock();
        /************************************************************************************************************/

        //�����߻�����
//        Point a;
//        for (uint8 i = USE_IMAGE_H - 1; i > 0; i--)//bgr
//        {
//            a.x = left_line_x[i];
//            a.y = i;
//            circle(use_mat, a, 0, Scalar(0, 255, 0), -1); //�������������Ϊ-1���������Ǹ�ʵ�㡣
//            a.x = right_line_x[i];
//            a.y = i;
//            circle(use_mat, a, 0, Scalar(0, 0, 255), -1);
//            a.x = center_line_x[i];
//            a.y = i;
//            circle(use_mat, a, 0, Scalar(255, 0, 0), -1);
//        }
        //��������
//        memcpy(left_line_x,left_border,sizeof(uint8)*USE_IMAGE_H);
//        memcpy(right_line_x,right_border,sizeof(uint8)*USE_IMAGE_H);
//        memset(center_line_x,USE_IMAGE_W/2,sizeof(uint8)*USE_IMAGE_H);
        //��ʾͼ��
        imshow("use_img", use_mat);
        waitKey(0);//�ȴ����̰���
    }
    return 0;
}

void LCDDrawPoint(uint8 row, uint8 column)
{
    //����
    Point a;
    a.x = column; a.y = row;
    circle(use_mat, a, 0, Scalar(32, 255, 0), -1); //�������������Ϊ-1���������Ǹ�ʵ�㡣
	//��ʾͼ��
	imshow("use_img", use_mat);
	waitKey(0);//�ȴ����̰���
}