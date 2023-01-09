#include<opencv2/opencv.hpp>
#include<iostream>
#include <string>
#include "main.h"
#include "ImageConversion.h"
//#include "ImageBasic.h"
#include"ImageWR.h"
#include <cstring>
#include "ImageTrack.h"

using namespace cv;

uint8 mt9v03x_image[MT9V03X_H][MT9V03X_W];

int main()
{
	//��ʼ����͸��ͼ��ѹ��
	ImagePerspective_Init();
	ImageBorderInit();
	BlackBorder();
    for (int i = 2; i < 42; i++)
    {
        printf("l_line_count=%d,r_line_count=%d,",l_line_count,r_line_count);
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
        myPoint test[EDGELINE_LENGTH];
        blur_points(left_line,l_line_count,test,7);
        memcpy(left_line,test,2*EDGELINE_LENGTH*sizeof(uint8));
        TrackEdgeGetCenterLine('l');
        /************************************************************************************************************/

        //�����߻�����
        Point a;
        for (uint8 i = 3; i < l_line_count-3; i++)//bgr
        {
            a.x = left_line[i].X;a.y = left_line[i].Y;
            circle(use_mat, a, 0, Scalar(0, 255, 0), -1); //�������������Ϊ-1���������Ǹ�ʵ�㡣
            a.x = center_line[i].X;a.y = center_line[i].Y;
            circle(use_mat, a, 0, Scalar(100, 0, 100), -1); //�������������Ϊ-1���������Ǹ�ʵ�㡣
        }
        for (uint8 i = r_line_count - 1; i > 0; i--)//bgr
        {
            a.x = right_line[i].X;a.y = right_line[i].Y;
            circle(use_mat, a, 0, Scalar(255, 255, 0), -1); //�������������Ϊ-1���������Ǹ�ʵ�㡣
        }
        printf("ls-lostline=%d,r_lostline=%d\n",l_lostline_num,r_lostline_num);
        //��ʾͼ��
        imshow("use_img", use_mat);
        waitKey(0);//�ȴ����̰���

        TrackBasicClear();

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