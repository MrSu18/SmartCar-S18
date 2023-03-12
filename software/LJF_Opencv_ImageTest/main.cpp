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

int main()
{
	//��ʼ����͸��ͼ��ѹ��
    ImagePerspective_Init();
	ImageBorderInit();
    for (int i = 0; i < 35; i++)
    {
        /******************************************������ȡ�Ҷ�ͼ***************************************/
        String str = format("E:\\nodeanddata\\studio\\FSL\\Complete\\S18\\data\\image\\3.10\\%d.bmp", i);
        //String str = "E:\\nodeanddata\\studio\\FSL\\Complete\\S18\\data\\image\\11.30\\10.bmp";
        ScanImage(str);
        /************************************************************************************************/

        //��ֵ��
        ImageBinary();
        UseImageDataToUseMat();
        PrintImage(use_mat);
        ImageProcess();

        printf("image_bias:%f,l_line_count:%d,r_line_count:%d\r\n",image_bias,l_line_count,r_line_count);
        /************************************************************************************************************/

        //�����߻�����
        PrintEdgeLine(f_left_line,0,l_line_count,0,255,0);
        PrintEdgeLine(f_right_line,0,r_line_count,0,255,255);
        PrintEdgeLine(center_line,0,c_line_count,255,0,0);

        //��ʾͼ��
        imshow("use_img", use_mat);
        waitKey(0);//�ȴ����̰���

        TrackBasicClear();

    }
    return 0;
}
