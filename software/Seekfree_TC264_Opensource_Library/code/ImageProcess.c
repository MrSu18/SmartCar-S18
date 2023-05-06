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

/*1:�󻷵� 2:�һ��� 3:ʮ�� 4:��· 5:�µ� 6:·�� 7:��� 'S':ͣ��*/
uint8 process_status[15]={1 ,3 ,4 ,3 ,5 ,7};//��״̬��Ԫ��ִ��˳������
uint16 process_speed[15]={70,70,60,70,70,70};//���������Ӧ��Ԫ��·�ε��ٶ�
uint8 process_status_cnt=0;//Ԫ��״̬����ļ�����

/***********************************************
* @brief : ͼ�����Ԫ��ʶ��
* @param : ��
* @return: ��
* @date  : 2023.3.6
* @author: ������
************************************************/
void ImageProcess(void)
{
    static uint8 status=5;
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

    //Ԫ��״̬��
//    if(CrossIdentify()==1)
//    {
//        pit_disable(CCU60_CH0);//�رյ���ж�
//        pit_disable(CCU60_CH1);
//        MotorSetPWM(0,0);
//    }
#if 1
    switch(status)
    {
        case 1:
        {
            if(CircleIslandLStatus()==1)
            {
               gpio_toggle_level(P21_3);
//               pit_disable(CCU60_CH0);//�رյ���ж�
//               pit_disable(CCU60_CH1);
//               MotorSetPWM(0,0);
               status=2;
            }
            break;
        }
        case 2:
        {
            if(CrossIdentify() == 1)
            {
                gpio_toggle_level(P21_3);
                status = 3;
                speed_type=kNormalSpeed;
                last_track_mode = track_mode;
                track_mode = kTrackADC;
                base_speed = 60;
            }
            break;
        }
        case 3:
        {
            if(CutIdentify() == 1)
            {
                gpio_toggle_level(P21_3);
                status = 4;
            }
            break;
        }
        case 4:
        {
            if(CrossIdentify() == 1)
            {
//                pit_disable(CCU60_CH0);//�رյ���ж�
//                pit_disable(CCU60_CH1);
//                MotorSetPWM(0,0);
                gpio_toggle_level(P21_3);
                status = 5;
                speed_type=kImageSpeed;
            }
            break;
        }
        case 5:
        {
            gpio_set_level(P21_4,GPIO_LOW);
            if(SlopeIdentify() == 1)
            {
                gpio_toggle_level(P21_3);
                status = 6;
            }
            break;
        }
        case 6:
        {
            gpio_set_level(P20_9,GPIO_LOW);
            if(GarageIdentify_L() == 1)
            {
//                pit_disable(CCU60_CH0);//�رյ���ж�
//                pit_disable(CCU60_CH1);
//                MotorSetPWM(0,0);
            }
            break;
        }
        default:break;
    }
#endif
#if 0
    switch(process_status[process_status_cnt])
    {
        case 1://�󻷵�
            if(CircleIslandLStatus()==1)
            {
                gpio_toggle_level(P21_3);
                process_status_cnt++;
                original_speed=process_speed[process_status_cnt];
            }
            break;
        case 2://�һ���
            break;
        case 3://ʮ��
            if(CrossIdentify()==1)
            {
                gpio_toggle_level(P21_3);
                if(process_status_cnt==1)
                {
                    speed_type=kNormalSpeed;
                    last_track_mode = track_mode;
                    track_mode = kTrackADC;
                    base_speed = 60;
                }
                process_status_cnt++;
                original_speed=process_speed[process_status_cnt];
            }
            break;
        case 4://��·
            if(CutIdentify()==1)
            {
                gpio_toggle_level(P21_3);
                process_status_cnt++;
                original_speed=process_speed[process_status_cnt];
            }
            break;
        case 5://�µ�
            if(SlopeIdentify()==1)
            {
                gpio_toggle_level(P21_3);
                process_status_cnt++;
                original_speed=process_speed[process_status_cnt];
            }
            break;
        case 6://·��
            break;
        case 7://����
            if(GarageIdentify_L()==1)
            {
                gpio_toggle_level(P21_3);
//                process_status_cnt++;
//                original_speed=process_speed[process_status_cnt];
            }
            break;
        case 'S':
            pit_disable(CCU60_CH1);
            pit_disable(CCU60_CH0);//�رյ���ж�
            MotorSetPWM(0,0);
            break;
        default:break;
    }
#endif

    //Ԥ�����ƫ��
    if(track_type==kTrackRight)
    {
        image_bias = GetAnchorPointBias(aim_distance, per_r_line_count, center_line_r);
    }
    else if(track_type==kTrackLeft)
    {
        image_bias = GetAnchorPointBias(aim_distance, per_l_line_count, center_line_l);
    }
    //�ٶȾ���
//    if(speed_type==kImageSpeed)
//    {
//        base_speed=SpeedDecision(original_speed,5);//�����68ֱ����80
//    }
//    tft180_show_uint(0, 0, base_speed, 3);
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

    if(over_count>=MT9V03X_W-2 && adc_sum < 10)                             //���ȫ��������ֵ��ֹͣ
    {
        pit_disable(CCU60_CH0);//�رյ���ж�
        pit_disable(CCU60_CH1);
        MotorSetPWM(0,0);
//            printf("%f,%f,%f,%f\n",err_max,err_min,errc_max,errc_min);
    }

}
