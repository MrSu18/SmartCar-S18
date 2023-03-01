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

using namespace cv;

uint8 mt9v03x_image[MT9V03X_H][MT9V03X_W];

inline int Limit(int x, int low, int up)//��x���������޷�
{
    return x > up ? up : x < low ? low : x;
}
inline int Min(int a,int b) {return a<b?a:b;}//��ab��Сֵ

int main()
{
	//��ʼ����͸��ͼ��ѹ��
    ImagePerspective_Init();
	ImageBorderInit();
    for (int i = 1; i < 35; i++)
    {
        /******************************************������ȡ�Ҷ�ͼ***************************************/
        String str = format("E:\\nodeanddata\\studio\\FSL\\Complete\\S18\\data\\image\\140��ʦ������ͷ2.20\\%d.bmp", i);
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
        BlurPoints(left_line, l_line_count, f_left_line, 3);
        BlurPoints(right_line, r_line_count, f_right_line, 3);
        //�Ⱦ����
        myPoint_f f_left_line1[EDGELINE_LENGTH],f_right_line1[EDGELINE_LENGTH];
        int l_count=200,r_count=200;
        ResamplePoints(f_left_line, l_line_count, f_left_line1, &l_count, 0.04*50);
        ResamplePoints(f_right_line, r_line_count, f_right_line1, &r_count, 0.04*50);
        //�ֲ�����
        float l_angle[l_count],r_angle[r_count];
        local_angle_points(f_left_line1,l_count,l_angle,0.1/0.04);
        local_angle_points(f_right_line1,r_count,r_angle,0.1/0.04);
        //���ʼ���ֵ����
        float l_angle_1[l_count],r_angle_1[r_count];
        nms_angle(l_angle,l_count,l_angle_1,(0.1/0.04)*2+1);
        nms_angle(r_angle,r_count,r_angle_1,(0.1/0.04)*2+1);
        //��������
        track_leftline(f_left_line1, l_count, center_line_l, (int) round(0.1/0.04), 50*(0.45/2));
        track_rightline(f_right_line1, r_count, center_line_r, (int) round(0.1/0.04), 50*(0.45/2));
        cl_line_count=l_count;cr_line_count=r_count;
        // Ԥ�����ƫ��
        float Bias=GetAnchorPointBias(0.3,cl_line_count,center_line_l);
        printf("%f\r\n",Bias);
        /************************************************************************************************************/


        //�����߻�����
        PrintEdgeLine(f_left_line1,0,l_count,0,255,0);
        PrintEdgeLine(f_right_line1,0,r_count,0,255,255);
        PrintEdgeLine(center_line,0,c_line_count,255,0,0);
//        printf("ls-lostline=%d,r_lostline=%d\n",l_lostline_num,r_lostline_num);
        //��ʾͼ��
        imshow("use_img", use_mat);
        waitKey(0);//�ȴ����̰���

        TrackBasicClear();

    }
    return 0;
}
