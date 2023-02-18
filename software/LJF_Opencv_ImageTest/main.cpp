#include<opencv2/opencv.hpp>
#include<iostream>
#include <string>
#include "main.h"
#include "ImageConversion.h"
#include "ImageBasic.h"
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
    for (int i = 0; i < 42; i++)
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
        //�Ա��߽����˲�
        myPoint_f f_left_line[EDGELINE_LENGTH],f_right_line[EDGELINE_LENGTH];
        BlurPoints(left_line, l_line_count, f_left_line, 7);
        BlurPoints(right_line, r_line_count, f_right_line, 7);
        //�Ⱦ����
        myPoint_f f_left_line1[EDGELINE_LENGTH],f_right_line1[EDGELINE_LENGTH];
        int l_count=200,r_count=200;
        ResamplePoints(f_left_line, l_line_count, f_left_line1, &l_count, 2);
        ResamplePoints(f_right_line, r_line_count, f_right_line1, &r_count, 2);
        //�ֲ�����
        float l_angle[l_count];
        local_angle_points(f_left_line1,l_count,l_angle,5);
        float l_angle_1[l_count];
        nms_angle(l_angle,l_count,l_angle_1,5*2+1);
        //��������
        track_leftline(f_left_line1, l_count, center_line, 10, 22.5);
        /************************************************************************************************************/

        for (int j = 0; j < l_count; ++j)
        {
            if (-1.57<=l_angle_1[j] && l_angle_1[j]<=-1)
            {
                printf("l_angle_1[%d]=%f\n",j,l_angle_1[j]);
                LCDDrawPoint(f_left_line1[j].Y,f_left_line1[j].X,0,255,0);
            }
        }

        //�����߻�����
//        PrintEdgeLine(f_left_line1,0,l_count,0,255,0);
//        PrintEdgeLine(f_right_line1,0,r_count,0,255,255);
//        PrintEdgeLine(center_line,0,l_count,255,0,0);
//        printf("ls-lostline=%d,r_lostline=%d\n",l_lostline_num,r_lostline_num);
        //��ʾͼ��
        imshow("use_img", use_mat);
        waitKey(0);//�ȴ����̰���

        TrackBasicClear();

    }
    return 0;
}
