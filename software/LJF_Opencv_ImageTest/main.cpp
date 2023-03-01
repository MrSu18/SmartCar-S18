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

inline int Limit(int x, int low, int up)//给x设置上下限幅
{
    return x > up ? up : x < low ? low : x;
}
inline int Min(int a,int b) {return a<b?a:b;}//求ab最小值

int main()
{
	//初始化逆透视图像并压缩
    ImagePerspective_Init();
	ImageBorderInit();
    for (int i = 0; i < 35; i++)
    {
        /******************************************总钻风获取灰度图***************************************/
        String str = format("E:\\nodeanddata\\studio\\FSL\\Complete\\S18\\data\\image\\140度师弟摄像头2.20\\%d.bmp", i);
        //String str = "E:\\nodeanddata\\studio\\FSL\\Complete\\S18\\data\\image\\11.30\\10.bmp";
        ScanImage(str);
        /************************************************************************************************/

        //二值化
        ImageBinary();
        UseImageDataToUseMat();
        PrintImage(use_mat);
        //扫线
        EdgeDetection();
        //对边线进行滤波
        myPoint_f f_left_line[EDGELINE_LENGTH],f_right_line[EDGELINE_LENGTH];
        BlurPoints(left_line, l_line_count, f_left_line, 3);
        BlurPoints(right_line, r_line_count, f_right_line, 3);
        //等距采样
        myPoint_f f_left_line1[EDGELINE_LENGTH],f_right_line1[EDGELINE_LENGTH];
        int l_count=200,r_count=200;
        ResamplePoints(f_left_line, l_line_count, f_left_line1, &l_count, 0.04*50);
        ResamplePoints(f_right_line, r_line_count, f_right_line1, &r_count, 0.04*50);
        //局部曲率
        float l_angle[l_count],r_angle[r_count];
        local_angle_points(f_left_line1,l_count,l_angle,0.1/0.04);
        local_angle_points(f_right_line1,r_count,r_angle,0.1/0.04);
        //曲率极大值抑制
        float l_angle_1[l_count],r_angle_1[r_count];
        nms_angle(l_angle,l_count,l_angle_1,(0.1/0.04)*2+1);
        nms_angle(r_angle,r_count,r_angle_1,(0.1/0.04)*2+1);
        //跟踪左线
        track_leftline(f_left_line1, l_count, center_line_l, (int) round(0.1/0.04), 50*(0.45/2));
        track_rightline(f_right_line1, r_count, center_line_r, (int) round(0.1/0.04), 48.8*(0.45/2));
        cl_line_count=l_count;cr_line_count=r_count;
        // 预瞄距离,动态效果更佳
        float aim_distance = 0.35;
        // 车轮对应点(纯跟踪起始点)
        float cx = USE_IMAGE_W/2;
        float cy = USE_IMAGE_H;

        // 找最近点(起始点中线归一化)
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
        // 中线有点，同时最近点不是最后几个点
        if (begin_id >= 0 && cl_line_count - begin_id >= 3)
        {
            // 归一化中线
            center_line_l[begin_id].X = cx;
            center_line_l[begin_id].Y = cy;
            c_line_count = sizeof(center_line) / sizeof(center_line[0]);
            ResamplePoints(center_line_l + begin_id, cl_line_count - begin_id, center_line, &c_line_count, 0.04*50);

            // 远预锚点位置
            int aim_idx = (round(aim_distance / 0.04), 0, c_line_count - 1);
            // 近预锚点位置
            int aim_idx_near = Limit(round(0.25 / 0.04), 0, c_line_count - 1);

            // 计算远锚点偏差值
            float dx = center_line[aim_idx].X - cx;
            float dy = cy - center_line[aim_idx].Y + 0.2 * 50;
            float dn = sqrt(dx * dx + dy * dy);
            float error = -atan2f(dx, dy) * 180 / 3.14;
            assert(!isnan(error));

            // 若考虑近点远点,可近似构造Stanley算法,避免撞路肩
            // 计算近锚点偏差值
            float dx_near = center_line[aim_idx_near].X - cx;
            float dy_near = cy - center_line[aim_idx_near].Y + 0.2 * 50;
            float dn_near = sqrt(dx_near * dx_near + dy_near * dy_near);
            float error_near = -atan2f(dx_near, dy_near) * 180 / 3.14;
            assert(!isnan(error_near));

            // 远近锚点综合考虑
            //angle = pid_solve(&servo_pid, error * far_rate + error_near * (1 - far_rate));
            // 根据偏差进行PD计算
            //float angle = pid_solve(&servo_pid, error);

            // 纯跟踪算法(只考虑远点)
            float pure_angle = -atanf(50 * 2 * 0.2 * dx / dn / dn) / 3.14 * 180 / 2.4;
            printf("%f\r\n",pure_angle);
//            float angle = pid_solve(&servo_pid, pure_angle);
//            float angle = MINMAX(angle, -14.5, 14.5);
        }
        else
        {
            // 中线点过少(出现问题)，则不控制舵机
            c_line_count = 0;
        }
        /************************************************************************************************************/


        //把三线画出来
        PrintEdgeLine(f_left_line1,0,l_count,0,255,0);
        PrintEdgeLine(f_right_line1,0,r_count,0,255,255);
        PrintEdgeLine(center_line_l,0,cl_line_count,255,0,0);
//        printf("ls-lostline=%d,r_lostline=%d\n",l_lostline_num,r_lostline_num);
        //显示图像
        imshow("use_img", use_mat);
        waitKey(0);//等待键盘按下

        TrackBasicClear();

    }
    return 0;
}
