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
#include "debug.h"
#include "icm20602.h"
#include "motor.h"

//���ִ����ٿ�����72����70���ڼ��ٵ�75�����ǰ���ٵ�65��ת��PΪ12DΪ1
/*1:�󻷵� 2:�һ��� 3:ʮ�� 4:��· 5:�µ� 6:·�� 7:����� 8:���ҿ� 9:��ֱ�� 'S':ͣ�� 'E':���������1m 'G':�����ǻ���70��*/
uint8 process_status[PROCESS_LENGTH]={3,3,3,3,5,2,1,3,4,6,5,3,3,3,7,'S'};//��״̬��Ԫ��ִ��˳������
uint16 process_speed[PROCESS_LENGTH]={60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60};//���������Ӧ��Ԫ��·�ε��ٶ�
ProcessProperty process_property[PROCESS_LENGTH];
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
    static uint8 encoder_status=0,gyro_status=0;;
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
#if 1
    switch(process_status[process_status_cnt])
    {
        case 1://�󻷵�
            if(CircleIslandLStatus()==1)
            {
                process_status_cnt++;
                original_speed=process_speed[process_status_cnt];
                base_speed=original_speed;
            }
            break;
        case 2://�һ���
            if(CircleIslandRStatus()==1)
            {
                process_status_cnt++;
                original_speed=process_speed[process_status_cnt];
                base_speed=original_speed;
            }
            break;
        case 3://ʮ��
            if(CrossIdentify()==1)
            {
                process_status_cnt++;
                original_speed=process_speed[process_status_cnt];
                base_speed=original_speed;
            }
            break;
        case 4://��·
            if(CutIdentify()==1)
            {
                process_status_cnt++;
                original_speed=process_speed[process_status_cnt];
                base_speed=original_speed;
            }
            break;
        case 5://�µ�
            if(SlopeIdentify()==1)
            {
                process_status_cnt++;
                original_speed=process_speed[process_status_cnt];
                base_speed=original_speed;
            }
            break;
        case 6://·��
            if(BarrierIdentify()==1)
            {
                process_status_cnt++;
                original_speed=process_speed[process_status_cnt];
                base_speed=original_speed;
            }
            break;
        case 7://�󳵿�
            if(GarageIdentify_L()==1)
            {
                gpio_toggle_level(BEER);
            }
            break;
        case 8://�ҳ���
            if(GarageIdentify_R()==1)
            {
                gpio_toggle_level(BEER);
            }
            break;
        case 9://��ֱ��
            if(GarageIdentify_Straight()==1)
            {
                gpio_toggle_level(BEER);
                process_status_cnt++;
                original_speed=process_speed[process_status_cnt];
                base_speed=original_speed;
            }
            break;
        case 'S':
            pit_disable(CCU60_CH1);
            pit_disable(CCU60_CH0);//�رյ���ж�
            MotorSetPWM(0,0);
            break;
        case 'E':
            if(encoder_status==0)
            {
                encoder_status=1;
                encoder_dis_flag=1;//�������������
            }
            else
            {
                if(dis>1000)
                {
                    encoder_dis_flag=0;
                    encoder_status=0;
                    gpio_toggle_level(BEER);
                    process_status_cnt++;
                    original_speed=process_speed[process_status_cnt];
                    base_speed=original_speed;
                }
            }
            break;
        case 'G':
            if(gyro_status==0)
            {
                gyro_status=1;
                StartIntegralAngle_X(70);//���������ǻ�70��
            }
            else
            {
                if(icm_angle_x_flag==1)//���ֵ���
                {
                    gyro_status=0;
                    gpio_toggle_level(BEER);
                    process_status_cnt++;
                    original_speed=process_speed[process_status_cnt];
                    base_speed=original_speed;
                }
            }
            break;
        default:break;
    }
#endif
    //Ԥ�����ƫ��
    if(track_type==kTrackRight)
    {
        if(r_line_count<10 && (process_status[process_status_cnt]==1 || process_status[process_status_cnt]==2))//��������Ѱ���ߵ����ұ��߲�����ʱ���ұ�����ɨ��
        {
            RightLineDetectionAgain('y');
            EdgeLinePerspective(right_line,r_line_count,per_right_line);
            per_r_line_count=PER_EDGELINE_LENGTH;
            BlurPoints(per_right_line, r_line_count, f_right_line, LINE_BLUR_KERNEL);
            ResamplePoints(per_right_line, r_line_count, f_right_line1, &per_r_line_count, SAMPLE_DIST*PIXEL_PER_METER);
            track_rightline(f_right_line1, per_r_line_count , center_line_r, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
        }
        image_bias = GetAnchorPointBias(aim_distance, per_r_line_count, center_line_r);
    }
    else if(track_type==kTrackLeft)
    {
        if (l_line_count<10 && (process_status[process_status_cnt]==1 || process_status[process_status_cnt]==2))//��������Ѱ���ߵ�������߲�����ʱ���������ɨ��
        {
            LeftLineDetectionAgain('y');
            per_l_line_count=PER_EDGELINE_LENGTH;
            EdgeLinePerspective(left_line,l_line_count,per_left_line);
            BlurPoints(per_left_line, l_line_count, f_left_line, LINE_BLUR_KERNEL);
            ResamplePoints(f_left_line, l_line_count, f_left_line1, &per_l_line_count, SAMPLE_DIST*PIXEL_PER_METER);
            track_leftline(f_left_line1, per_l_line_count, center_line_l, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
        }
        image_bias = GetAnchorPointBias(aim_distance, per_l_line_count, center_line_l);
    }
    //�ٶȾ���
//    if(speed_type==kImageSpeed)
//    {
//        base_speed=SpeedDecision(original_speed,8);//�����68ֱ����80
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
    l_line_count=0;r_line_count=0;//���ߵļ���ָ������
    per_l_line_count=PER_EDGELINE_LENGTH,per_r_line_count=PER_EDGELINE_LENGTH;
    //�������Ƶ���������
    memset(l_growth_direction,0,8);
    memset(r_growth_direction,0,8);
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
        adc_sum += normalvalue[i];
    for(int16 i = 0;i < MT9V03X_W;i++)                       //�������һ��
    {
        if(mt9v03x_image[106][i] <= OUT_THRESHOLD)
                over_count++;
    }
    //���ȫ��������ֵ��ֹͣ
    if(over_count>=MT9V03X_W-2 && adc_sum < 10)
    {
        pit_disable(CCU60_CH0);//�رյ���ж�
        pit_disable(CCU60_CH1);
        pit_disable(CCU61_CH1);
        MotorSetPWM(0,0);
    }
}

/***********************************************
* @brief : ״̬���������Գ�ʼ
* @param : void
* @return: void
* @date  : 2023.7.8
* @author: ������
************************************************/
void ProcessPropertyInit(void)
{
    for(uint8 i=0;i<PROCESS_LENGTH;i++)
    {
        if(process_status[i]==7 || process_status[i]==8)//���
        {
            process_property[i].min_speed=60;
        }
        switch(process_status[i])
        {
            case 1://�󻷵�
                process_property[i].max_speed=65;
                process_property[i].min_speed=60;
                break;
            case 2://�һ���
                process_property[i].max_speed=60;
                process_property[i].min_speed=60;
                break;
            case 3://ʮ��
                process_property[i].max_speed=60;
                process_property[i].min_speed=60;
                break;
            case 4://��·
                process_property[i].max_speed=60;
                process_property[i].min_speed=62;
                break;
            case 5://�µ�
                process_property[i].max_speed=60;
                process_property[i].min_speed=55;
                break;
            case 6://·��
                process_property[i].max_speed=60;
                process_property[i].min_speed=60;
                break;
            case 7://�����
                process_property[i].max_speed=60;
                process_property[i].min_speed=60;
                break;
            default:break;
        }
    }
}

void ProcessPropertyDefault(uint8 i)
{
    if(process_status[i]==7 || process_status[i]==8)//���
    {
        process_property[i].min_speed=60;
    }
    switch(process_status[i])
    {
        case 1://�󻷵�
            process_property[i].max_speed=65;
            process_property[i].min_speed=60;
            break;
        case 2://�һ���
            process_property[i].max_speed=60;
            process_property[i].min_speed=60;
            break;
        case 3://ʮ��
            process_property[i].max_speed=60;
            process_property[i].min_speed=60;
            break;
        case 4://��·
            process_property[i].max_speed=60;
            process_property[i].min_speed=60;
            break;
        case 5://�µ�
            process_property[i].max_speed=60;
            process_property[i].min_speed=55;
            break;
        case 6://·��
            process_property[i].max_speed=60;
            process_property[i].min_speed=60;
            break;
        case 7://�����
            process_property[i].max_speed=60;
            process_property[i].min_speed=60;
            break;
        default:break;
    }
}
