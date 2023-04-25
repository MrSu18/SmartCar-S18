/*
 * ImageProcess.c
 *
 *  Created on: 2023��3��1��
 *      Author: L
 */
#include "ImageProcess.h"
#include "zf_device_tft180.h"
#include "zf_driver_pit.h"
#include "ImageProcess.h"
#include "ImageConversion.h"
#include "ImageBasic.h"
#include "ImageTrack.h"
#include "math.h"
#include "ImageSpecial.h"
#include <string.h>
#include "pid.h"
#include "Control.h"

/***********************************************
* @brief : ���籣������ͨ������ͼ�����һ�е����ص�
* @param : void
* @return: void
* @date  : 2023.3.1
* @author: L & ������
************************************************/
void OutProtect(void)
{
    int over_count = 0;                                   //��ų�����ֵ�����ص������
    int adc_sum = 0;

    for(int16 i = 0;i < 5;i++)
        adc_sum += adc_value[i];
    adc_sum -= adc_value[2];
    for(int16 i = 0;i < MT9V03X_W;i++)                       //�������һ��
    {
        if(mt9v03x_image[106][i] <= OUT_THRESHOLD)
                over_count++;
    }

    if(over_count >= (MT9V03X_W - 2) && (adc_sum < 10))                             //���ȫ��������ֵ��ֹͣ
    {
        pit_disable(CCU60_CH0);//�رյ���ж�
        pit_disable(CCU60_CH1);
        MotorSetPWM(0,0);
//            printf("%f,%f,%f,%f\n",err_max,err_min,errc_max,errc_min);
    }

}


/***********************************************
* @brief : ͼ�����Ԫ��ʶ��
* @param : ��
* @return: ��
* @date  : 2023.3.6
* @author: ������
************************************************/
void ImageProcess(void)
{
    static uint8 status=1;
    //ɨ��
    EdgeDetection();
    //���߽���͸��
    EdgeLinePerspective(left_line,l_line_count,per_left_line);
    EdgeLinePerspective(right_line,r_line_count,per_right_line);
    //�Ա��߽����˲�
    BlurPoints(per_left_line, l_line_count, f_left_line, LINE_BLUR_KERNEL);
    BlurPoints(per_right_line, r_line_count, f_right_line, LINE_BLUR_KERNEL);
    //�Ⱦ�������������Ƕ�͸��ͼ�����ɨ�����Բ���Ҫ���еȾ����
    ResamplePoints(f_left_line, l_line_count, f_left_line1, &per_l_line_count, SAMPLE_DIST*PIXEL_PER_METER);
    ResamplePoints(f_right_line, r_line_count, f_right_line1, &per_r_line_count, SAMPLE_DIST*PIXEL_PER_METER);
    //�ֲ�����
    local_angle_points(f_left_line1,per_l_line_count,l_angle,ANGLE_DIST/SAMPLE_DIST);
    local_angle_points(f_right_line1,per_r_line_count,r_angle,ANGLE_DIST/SAMPLE_DIST);
    //���ʼ���ֵ����
    nms_angle(l_angle,per_l_line_count,l_angle_1,(ANGLE_DIST/SAMPLE_DIST)*2+1);
    nms_angle(r_angle,per_r_line_count,r_angle_1,(ANGLE_DIST/SAMPLE_DIST)*2+1);
    //��������
    track_leftline(f_left_line1, per_l_line_count, center_line_l, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
    track_rightline(f_right_line1, per_r_line_count , center_line_r, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));

    //�л�����Ѳ��
    if(per_r_line_count > 2 && per_l_line_count < 20)
    {
        track_type=kTrackRight;
    }
    else if(per_l_line_count>2 && per_r_line_count < 20)
    {
        track_type=kTrackLeft;
    }
    else if(per_l_line_count < 2 && per_r_line_count > 2)
    {
        track_type = kTrackRight;
    }
    else if(per_l_line_count > 2 && per_r_line_count < 2)
    {
        track_type=kTrackLeft;
    }
    else
    {
        track_type=kTrackRight;
    }

    switch(status)
    {
        case 1:
            if(CircleIslandLStatus()==1)
            {
                pit_disable(CCU60_CH1);
                pit_disable(CCU60_CH0);//�رյ���ж�
                MotorSetPWM(0,0);
//               status=2;
            }
            break;
        case 2:
            if(CrossIdentify() == 1)
            {
//                status = 3;
                pit_disable(CCU60_CH1);
                pit_disable(CCU60_CH0);//�رյ���ж�
                MotorSetPWM(0,0);
            }
            break;
        case 3:
            if(CutIdentify() == 1)
            {
                pit_disable(CCU60_CH0);//�رյ���ж�
                pit_disable(CCU60_CH1);
                MotorSetPWM(0,0);
            }
            break;
        default:break;
    }
//    uint8 temp=CircleIslandLIn();
//    tft180_show_uint(0, 0, temp, 2);
    //Ԥ�����ƫ��
    if(track_type==kTrackRight)
    {
        image_bias = GetAnchorPointBias(aim_distance, per_r_line_count, center_line_r);
    }
    else if(track_type==kTrackLeft)
    {
        image_bias = GetAnchorPointBias(aim_distance, per_l_line_count, center_line_l);
    }
//    base_speed=SpeedDecision(150,30);
}

/***********************************************
* @brief : ����������Ϣ�������ã�Ϊ��һ֡��׼��
* @param : ȫ�ֱ���
* @return: ��ʼ��Ϊ0��ȫ�ֱ���
* @date  : 2023.3.6
* @author: ������
* @note  :  ��
************************************************/
void TrackBasicClear(void)
{
    //���߶�����������
    memset(l_lost_line,LOST_LINE_FALSE,sizeof(char)*EDGELINE_LENGTH);
    memset(r_lost_line,LOST_LINE_FALSE,sizeof(char)*EDGELINE_LENGTH);
    l_line_count=0;r_line_count=0;//���ߵļ���ָ������
    per_l_line_count=PER_EDGELINE_LENGTH,per_r_line_count=PER_EDGELINE_LENGTH;
    l_lostline_num=0;r_lostline_num=0;//����������
}
