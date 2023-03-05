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

int main()
{
	//初始化逆透视图像并压缩
    ImagePerspective_Init();
	ImageBorderInit();
    for (int i = 5; i < 35; i++)
    {
        /******************************************总钻风获取灰度图***************************************/
        String str = format("E:\\nodeanddata\\studio\\FSL\\Complete\\S18\\data\\image\\140无极变摄像头2.25\\%d.bmp", i);
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
        track_rightline(f_right_line1, r_count, center_line_r, (int) round(0.1/0.04), 50*(0.45/2));
        cl_line_count=l_count;cr_line_count=r_count;

        float Bias=0;

        // 单侧线少，切换巡线方向  切外向圆
        if (l_count < r_count / 2 && l_count < 10)
        {
            Bias=GetAnchorPointBias(0.4,cr_line_count,center_line_r);
        }
        else if (r_count < l_count / 2 && r_count < 10)
        {
            Bias=GetAnchorPointBias(0.4,cl_line_count,center_line_l);
        }
        else if (l_count < 5 && r_count > l_count)
        {
            Bias=GetAnchorPointBias(0.4,cr_line_count,center_line_r);
        }
        else if (r_count < 5 && l_count > r_count)
        {
            Bias=GetAnchorPointBias(0.4,cl_line_count,center_line_l);
        }
        else
        {
            Bias=GetAnchorPointBias(0.4,cr_line_count,center_line_r);
        }

        // 预瞄点求偏差

        printf("%f\r\n",Bias);
        /************************************************************************************************************/


        //把三线画出来
        PrintEdgeLine(f_left_line1,0,l_count,0,255,0);
        PrintEdgeLine(f_right_line1,0,r_count,0,255,255);
        PrintEdgeLine(center_line,0,c_line_count,255,0,0);
//        printf("ls-lostline=%d,r_lostline=%d\n",l_lostline_num,r_lostline_num);
        //显示图像
        imshow("use_img", use_mat);
        waitKey(0);//等待键盘按下

        TrackBasicClear();

    }
    return 0;
}
