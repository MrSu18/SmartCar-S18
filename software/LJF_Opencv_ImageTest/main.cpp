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
    for (int i = 0; i < 35; i++)
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
        track_rightline(f_right_line1, r_count, center_line_r, (int) round(0.1/0.04), 48.8*(0.45/2));
        cl_line_count=l_count;cr_line_count=r_count;
        // Ԥ�����,��̬Ч������
        float aim_distance = 0.35;
        // ���ֶ�Ӧ��(��������ʼ��)
        float cx = USE_IMAGE_W/2;
        float cy = USE_IMAGE_H;

        // �������(��ʼ�����߹�һ��)
        float min_dist = 1e10;
        int begin_id = -1;
        for (int i = 0; i < cl_line_count; i++)
        {
            float dx = center_line_l[i].X - cx;
            float dy = center_line_l[i].Y - cy;
            float dist = sqrt(dx * dx + dy * dy);
            if (dist < min_dist)
            {
                min_dist = dist;
                begin_id = i;
            }
        }
        // �����е㣬ͬʱ����㲻����󼸸���
        if (begin_id >= 0 && cl_line_count - begin_id >= 3)
        {
            // ��һ������
            center_line_l[begin_id].X = cx;
            center_line_l[begin_id].Y = cy;
            c_line_count = sizeof(center_line) / sizeof(center_line[0]);
            ResamplePoints(center_line_l + begin_id, cl_line_count - begin_id, center_line, &c_line_count, 0.04*50);

            // ԶԤê��λ��
            int aim_idx = (round(aim_distance / 0.04), 0, c_line_count - 1);
            // ��Ԥê��λ��
            int aim_idx_near = Limit(round(0.25 / 0.04), 0, c_line_count - 1);

            // ����Զê��ƫ��ֵ
            float dx = center_line[aim_idx].X - cx;
            float dy = cy - center_line[aim_idx].Y + 0.2 * 50;
            float dn = sqrt(dx * dx + dy * dy);
            float error = -atan2f(dx, dy) * 180 / 3.14;
            assert(!isnan(error));

            // �����ǽ���Զ��,�ɽ��ƹ���Stanley�㷨,����ײ·��
            // �����ê��ƫ��ֵ
            float dx_near = center_line[aim_idx_near].X - cx;
            float dy_near = cy - center_line[aim_idx_near].Y + 0.2 * 50;
            float dn_near = sqrt(dx_near * dx_near + dy_near * dy_near);
            float error_near = -atan2f(dx_near, dy_near) * 180 / 3.14;
            assert(!isnan(error_near));

            // Զ��ê���ۺϿ���
            //angle = pid_solve(&servo_pid, error * far_rate + error_near * (1 - far_rate));
            // ����ƫ�����PD����
            //float angle = pid_solve(&servo_pid, error);

            // �������㷨(ֻ����Զ��)
            float pure_angle = -atanf(50 * 2 * 0.2 * dx / dn / dn) / 3.14 * 180 / 2.4;
            printf("%f\r\n",pure_angle);
//            float angle = pid_solve(&servo_pid, pure_angle);
//            float angle = MINMAX(angle, -14.5, 14.5);
        }
        else
        {
            // ���ߵ����(��������)���򲻿��ƶ��
            c_line_count = 0;
        }
        /************************************************************************************************************/


        //�����߻�����
        PrintEdgeLine(f_left_line1,0,l_count,0,255,0);
        PrintEdgeLine(f_right_line1,0,r_count,0,255,255);
        PrintEdgeLine(center_line_l,0,cl_line_count,255,0,0);
//        printf("ls-lostline=%d,r_lostline=%d\n",l_lostline_num,r_lostline_num);
        //��ʾͼ��
        imshow("use_img", use_mat);
        waitKey(0);//�ȴ����̰���

        TrackBasicClear();

    }
    return 0;
}
