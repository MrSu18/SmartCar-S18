//
// Created by 30516 on 2023/3/6.
//
#include "ImageProcess.h"
#include "ImageConversion.h"
#include "ImageBasic.h"
#include "ImageTrack.h"
#include "math.h"
#include <string.h>
#include "ImageSpecial.h"

/***********************************************
* @brief : ͼ������Ԫ��ʶ��
* @param : ��
* @return: ��
* @date  : 2023.3.6
* @author: ������
************************************************/
void ImageProcess(void)
{
    //ɨ��
    EdgeDetection();
    //�Ա��߽����˲�
    BlurPoints(left_line, l_line_count, f_left_line, LINE_BLUR_KERNEL);
    BlurPoints(right_line, r_line_count, f_right_line, LINE_BLUR_KERNEL);
    //�Ⱦ�������������Ƕ�͸��ͼ�����ɨ�����Բ���Ҫ���еȾ����
//    ResamplePoints(f_left_line, l_line_count, f_left_line1, &l_count, SAMPLE_DIST*PIXEL_PER_METER);
//    ResamplePoints(f_right_line, r_line_count, f_right_line1, &r_count, SAMPLE_DIST*PIXEL_PER_METER);
    //�ֲ�����
    local_angle_points(f_left_line,l_line_count,l_angle,ANGLE_DIST/SAMPLE_DIST);
    local_angle_points(f_right_line,r_line_count,r_angle,ANGLE_DIST/SAMPLE_DIST);
    //���ʼ���ֵ����
    nms_angle(l_angle,l_line_count,l_angle_1,(ANGLE_DIST/SAMPLE_DIST)*2+1);
    nms_angle(r_angle,r_line_count,r_angle_1,(ANGLE_DIST/SAMPLE_DIST)*2+1);
    //��������
    track_leftline(f_left_line, l_line_count, center_line_l, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
    track_rightline(f_right_line, r_line_count, center_line_r, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
    CircleIslandRStatus();

    //�л�����Ѳ��
    if(l_line_count < 20)
    {
        track_type=kTrackRight;
    }
    else if(r_line_count < 20)
    {
        track_type=kTrackLeft;
    }
    else
    {
        track_type=kTrackRight;
    }

    //Ԥ�����ƫ��
    if(track_type==kTrackRight)
    {
        image_bias = GetAnchorPointBias(aim_distance, r_line_count, center_line_r);
    }
    else if(track_type==kTrackLeft)
    {
        image_bias = GetAnchorPointBias(aim_distance, l_line_count, center_line_l);
    }
}

/***********************************************
* @brief : ����������Ϣ�������ã�Ϊ��һ֡��׼��
* @param : ȫ�ֱ���
* @return: ��ʼ��Ϊ0��ȫ�ֱ���
* @date  : 2023.3.6
* @author: ������
* @note	 :  ��
************************************************/
void TrackBasicClear(void)
{
    //���߶�����������
    memset(l_lost_line,LOST_LINE_FALSE,sizeof(char)*EDGELINE_LENGTH);
    memset(r_lost_line,LOST_LINE_FALSE,sizeof(char)*EDGELINE_LENGTH);
    l_line_count=0;r_line_count=0;//���ߵļ���ָ������
    l_lostline_num=0;r_lostline_num=0;//����������
}