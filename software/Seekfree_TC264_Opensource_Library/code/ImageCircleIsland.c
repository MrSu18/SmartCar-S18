//
// Created by 30516 on 2023/3/10.
//
#include "ImageSpecial.h"
#include "ImageTrack.h"
#include <stdio.h>
#include "zf_device_tft180.h"
#include "adc.h"
#include "stdlib.h"
#include "debug.h"
#include "motor.h"
#include "icm20602.h"
#include "Control.h"
#include "zf_driver_gpio.h"

//#define PI 3.1415926
#define CIRCLE_SIDE_LARGE_ADC_THR 2000 //�����˻������²�ʱ���������Ǳ߱�ADC��ֵӦ�ô��������ֵ
#define CIRCLE_SIDE_SMALL_ADC_THR 1000 //�����˻������²�ʱ����һ��ADC��ֵӦ��С���������ֵ
#define CIRCLE_MID_ADC_THR        3000 //�����˻������²�ʱ���м���Ӧ�ô��������ֵ
#define CIRCLE_SPECIAL_ADC_THR    4095 //��������·����ȫƽ��Զ�뻷������ʱ��˫�ߵ�л����4095���ҳ��ڻ����в���Ҫ��ʱ�뻷
#define CIRCLE_SPECIAL_ADC_THR2   4000 //��������·����ȫƽ��Զ�뻷������ʱ�򿿽������ڰ˻����3000���ҳ��ڻ����в���Ҫ��ʱ�뻷

/**************************************************�󻷵�***************************************************************/
uint8 CircleIslandLStatus()//��߻���״̬״̬��
{
    static uint8 status=0;//����״̬ת�Ʊ���
//    tft180_show_uint(120, 100, status, 2);
    switch (status)
    {
        case 0: //����󻷵�
            if(CircleIslandLDetection()==1)
            {
                speed_type=kNormalSpeed;//�ر��ٶȾ���
                base_speed=60;//���ٽ���
                gpio_set_level(BEER,1);
                status=1;
            }
            else if(L>=CIRCLE_SPECIAL_ADC_THR && LM>CIRCLE_SPECIAL_ADC_THR2)//�������ڳ�����ƽ��ƫ�뻷�����������������ʱ���Ӵ���ڻ����в��Ѿ���Ҫ�뻷����������״̬1
            {
                speed_type=kNormalSpeed;//�ر��ٶȾ���
                base_speed=60;//���ٽ���
                gpio_set_level(BEER,1);
                StartIntegralAngle_X(320);//����������׼�����ֳ���
                status=2;
            }
            break;
        case 1: //·��������һ�����
            if(CircleIslandLInDetection()==1)
            {
                StartIntegralAngle_X(320);//����������׼�����ֳ���
                status=2;
            }
            break;
        case 2: //���뻷��
            if(CircleIslandLIn()==1)
            {
//                base_speed=65;//���ڼ���
                status=3;
            }
            else  if (CircleIslandLOutFinish()==1)//��ֹ̫���ڶ��������⻷ʹ��״̬���ң������ǻ��ֵ�����ǿ�Ƴ���
            {
                status=5;
            }
            break;
        case 3: //������
            track_type=kTrackRight;//�ڻ���Ĭ��Ѱ��Բ
            if(CircleIslandLOutDetection()==1)
            {
                track_type=kTrackRight;
                status=4;
            }
            else  if (CircleIslandLOutFinish()==1)//��ֹ̫���ڶ��������⻷ʹ��״̬���ң������ǻ��ֵ�����ǿ�Ƴ���
            {
                status=5;
            }
            break;
        case 4://����
            if (CircleIslandLOutFinish()==1)
            {
                base_speed=original_speed;//�ָ��ٶ�
                status=5;
            }
            else CircleIslandLOut();//����û������һֱ����������
            break;
        case 5://��⻷���Ƿ����
            if (CircleIslandLEnd()==1)
            {
                base_speed=original_speed;//�ָ��ٶ�
                speed_type=kImageSpeed;//�����ٶȾ���
                gpio_set_level(BEER,0);
                status=0;
                return 1;
            }
            break;
        default:
            break;
    }
    return 0;
}

/***********************************************
* @brief : ���С���Ƿ��ߵ��˻���Ԫ�ز���
* @param : ��
* @return: 0����û��⵽��ǵ� 1����⵽����߽ǵ� 2����ǵ���ʧ
* @date  : 2023.5.26
* @author: ������
* @notice: �Ƕ�./180.*PI����Ϊ��õ������ǻ����ƵĲ��ǽǶ���
************************************************/
uint8 CircleIslandLDetection()//����󻷵�
{
    track_type=kTrackRight;//Ĭ��Ѱ�ұ�
    //�������²��ֵ������
    if((L>CIRCLE_SIDE_LARGE_ADC_THR || R>CIRCLE_SIDE_LARGE_ADC_THR) && M>CIRCLE_MID_ADC_THR)
    {
        return 1;
    }
    return 0;
}

/***********************************************
* @brief : ����Ƿ�����Ҫ�뻷��״̬
* @param : ��
* @return: �Ƿ��뻷 1���뻷 0���뻷�������뻷
* @date  : 2023.5.26
* @author: ������
************************************************/
uint8 CircleIslandLInDetection(void)
{
    static uint8 status=0;
    if(status==0)
    {
        encoder_dis_flag = 1;//��������������
        status=1;
    }
    else if(status==1)
    {
        if(dis>150)//20cm
        {
            encoder_dis_flag = 0;
            status=0;
            track_type=kTrackLeft;
            return 1;
        }
    }
    track_type=kTrackRight;//����û���뻷ʱ��
    return 0;
}

/***********************************************
* @brief : �󻷵��뻷״̬
* @param : ��
* @return: �Ƿ��뻷���� 1���뻷���� 0���뻷δ����
* @date  : 2023.4.15
* @author: ������
************************************************/
#define JUDGE_IN_EDD_THR    40//�ж��뻷״̬�������ұ�������X����Ĳ�ֵ��ֵ
uint8 CircleIslandLIn()//�뻷״̬
{
    //�������ж��Ƿ������뻷״̬
    int len=0;
    if (per_r_line_count>EDGELINE_LENGTH-10) len=EDGELINE_LENGTH-10;
    else                                     len=per_r_line_count;
    if(f_right_line1[0].X-f_right_line1[len-1].X>JUDGE_IN_EDD_THR && right_line[r_line_count-1].X<80)
    {
        return 1;//�뻷����
    }
    //���뻷״̬����ƫ���
    if(l_line_count<5)//���90�ж����ߣ�˵��ֻ��������߽�ȥ
    {
        //����ɨ��
        LeftLineDetectionAgain('n');
        //������ɨ�������ߵ��������Գ���λ�ý����ж�,Ȼ������߽��д���
        if(left_line[0].X<left_line[l_line_count-1].X && left_line[0].Y<100)//��߿�����Բ���ڻ���ֻ�ܿ���Բ���⻷
        {
            uint8 y=0;//flag����������
            r_line_count=1;
            for (uint8 i=0;i<l_line_count-2;i++)
            {
                if(left_line[l_line_count-1-i].Y>=y)
                {
                    y=left_line[l_line_count-1-i].Y;
                }
                else if (left_line[l_line_count-1-i].X>left_line[l_line_count-2-i].X)//��y��ʼ�������������һ�����x���������������
                {
                    right_line[r_line_count]=left_line[l_line_count-i];
                    r_line_count++;
                }
            }
//            l_line_count=0;per_l_line_count=0;
            per_r_line_count=PER_EDGELINE_LENGTH;
            //�Ա��߽���͸��
            EdgeLinePerspective(right_line,r_line_count,per_right_line);
            BlurPoints(per_right_line, r_line_count, f_right_line, LINE_BLUR_KERNEL);
            ResamplePoints(f_right_line, r_line_count, f_right_line1, &per_r_line_count, SAMPLE_DIST*PIXEL_PER_METER);
            track_rightline(f_right_line1, per_r_line_count, center_line_r, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
            track_type=kTrackRight;
        }
        else track_type=kTrackLeft;//���û����Ѱ����ߵ�Բ����ȥ
    }
    else
    {
        track_type=kTrackLeft;//���û����Ѱ����ߵ�Բ����ȥ
    }
    return 0;
}

/***********************************************
* @brief : �󻷵�������
* @param : ��
* @return: �Ƿ񵽳���״̬ 1���������� 0�����ڻ���
* @date  : 2023.4.15
* @author: ������
************************************************/
uint8 CircleIslandLOutDetection()//�󻷵�����״̬
{
    if (r_line_count<2)//�ұ�һ��ʼ�Ͷ���
    {
        //����ɨ��
        RightLineDetectionAgain('n');
        per_r_line_count=PER_EDGELINE_LENGTH;
        EdgeLinePerspective(right_line,r_line_count,per_right_line);
        BlurPoints(per_right_line, r_line_count, f_right_line, LINE_BLUR_KERNEL);
        ResamplePoints(f_right_line, r_line_count, f_right_line1, &per_r_line_count, SAMPLE_DIST*PIXEL_PER_METER);
        local_angle_points(f_right_line1,per_r_line_count,r_angle,ANGLE_DIST/SAMPLE_DIST);
        nms_angle(r_angle,per_r_line_count,r_angle_1,(ANGLE_DIST/SAMPLE_DIST)*2+1);
        track_rightline(f_right_line1, per_r_line_count, center_line_r, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
    }
    //���нǵ��ж�
    for (int i = 0; i < per_r_line_count; ++i)
    {
        if (70./180.*PI<r_angle_1[i] && r_angle_1[i]<140./180.*PI && i<0.8/SAMPLE_DIST)//�����ұ߽ǵ�
        {
            return 1;
        }
    }
    return 0;
}

/***********************************************
* @brief : �󻷵�����
* @param : ��
* @return: 0������δ���� 1����������
* @date  : 2023.4.16
* @author: ������
************************************************/
uint8 CircleIslandLOutFinish(void)//��⻷���Ƿ����
{
    if(icm_angle_x_flag==1)
    {
        return 1;
    }
    return 0;
}
void CircleIslandLOut(void)//��������������
{
    //��������
    if (l_line_count>2)//���������ܿ����߾�����Բ��ȥ
    {
        track_type=kTrackLeft;
    }
    else
    {
        if (r_line_count>4)
        {
            //�жϽǵ��λ��
            for (int i = 0; i < per_r_line_count; ++i)
            {
                if (70./180.*PI<r_angle_1[i] && r_angle_1[i]<140./180.*PI)//�����ұ߽ǵ�
                {
                    if (i>aim_distance/SAMPLE_DIST)
                    {
                        per_r_line_count=i;
                        track_type=kTrackRight;
                    }
                    else//�̳��ϴε�ƫ��
                    {
                        if (image_bias<5) image_bias=8;//����ϴ�̫С�͹̶�����
                        track_type=kTrackSpecial;
                    }
                    break;
                }
            }
        }
        else//���ұ��߶�����
        {
            r_line_count=0;//���¹滮�ұ���
            //�ҵ����ߵ����Ͻǵ�
            uint8 half=GRAY_BLOCK/2;
            myPoint left_seed=left_line[l_line_count-1];//������
            if (left_seed.Y==0)
            {
                left_seed.Y=USE_IMAGE_H_MAX-half-1,left_seed.X=half;
            }
            for (; left_seed.Y>USE_IMAGE_H_MIN; left_seed.Y--)
            {
                if(PointSobelTest(left_seed)==1) break;
            }
            myPoint right_seed;//������
            right_seed.Y=USE_IMAGE_H-half-1,right_seed.X=USE_IMAGE_W-half-2;//ͼ�����½�
            right_line[r_line_count]=right_seed;r_line_count++;//˳���ܴ���Ȼ�����ߵķ���ͷ���
            right_line[r_line_count]=left_seed;r_line_count++;
            //*****************************
            per_r_line_count=PER_EDGELINE_LENGTH;
            EdgeLinePerspective(right_line,r_line_count,per_right_line);
            ResamplePoints(per_right_line, r_line_count, f_right_line1, &per_r_line_count, SAMPLE_DIST*PIXEL_PER_METER);
            track_rightline(f_right_line1, per_r_line_count, center_line_r, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
            track_type=kTrackRight;
        }
    }
}

/***********************************************
* @brief : �ж��󻷵��Ƿ����
* @param : ��
* @return: ��
* @date  : 2023.3.19
* @author: ������
************************************************/
uint8 CircleIslandLEnd(void)
{
    if(l_line_count>10)//�ж�
    {
        if(left_line[10].X-left_line[0].X>5)//��������������ұ����������ƣ����������λ������ԶС
            return 1;
    }
    //ѭ��
    track_type=kTrackRight;
    return 0;
}
/**********************************************************************************************************************/

/**************************************************�һ���***************************************************************/
uint8 CircleIslandRStatus()//�ұ߻���״̬״̬��
{
    static uint8 status;//����״̬ת�Ʊ���
//    tft180_show_uint(120, 100, status, 2);
    switch (status)
    {
        case 0: //����һ���
            if(CircleIslandRDetection()==1)
            {
                speed_type=kNormalSpeed;//�ر��ٶȾ���
                base_speed=60;//���ٽ���
                gpio_set_level(BEER,1);//����������
                status=1;
            }
            else if(R>=CIRCLE_SPECIAL_ADC_THR && RM>CIRCLE_SPECIAL_ADC_THR2)//�������ڳ�����ƽ��ƫ�뻷�����������������ʱ���Ӵ���ڻ����в��Ѿ���Ҫ�뻷����������״̬1
            {
                speed_type=kNormalSpeed;//�ر��ٶȾ���
                base_speed=60;//���ٽ���
                gpio_set_level(BEER,1);
                StartIntegralAngle_X(320);//����������׼�����ֳ���
                status=2;
            }
            break;
        case 1: //·��������һ�����
            if(CircleIslandRInDetection()==1)
            {
                gpio_toggle_level(P21_3);
                StartIntegralAngle_X(320);//����������׼�����ֳ���
                status=2;
            }
            break;
        case 2: //���뻷��
            if(CircleIslandRIn()==1)
            {
//                base_speed=65;
                status=3;
            }
            else  if (CircleIslandROutFinish()==1)//��ֹ̫���ڶ��������⻷ʹ��״̬���ң������ǻ��ֵ�����ǿ�Ƴ���
            {
                status=5;
            }
            break;
        case 3: //������
            track_type=kTrackLeft;//�ڻ���Ĭ��Ѱ��Բ
            if(CircleIslandROutDetection()==1)
            {
                track_type=kTrackLeft;
                status=4;
            }
            else  if (CircleIslandROutFinish()==1)//��ֹ̫���ڶ��������⻷ʹ��״̬���ң������ǻ��ֵ�����ǿ�Ƴ���
            {
                status=5;
            }
            break;
        case 4://����
            if (CircleIslandROutFinish()==1)
            {
                base_speed=original_speed;//�ָ��ٶ�
                status=5;
            }
            else CircleIslandROut();//����û������һֱ����������
            break;
        case 5://��⻷���Ƿ����
            if (CircleIslandREnd()==1)
            {
                base_speed=original_speed;//�ָ��ٶ�
                speed_type=kImageSpeed;//�����ٶȾ���
                gpio_set_level(BEER,0);
                status=0;
                return 1;
            }
            break;
        default:
            break;
    }
    return 0;
}

/***********************************************
* @brief : ���С���Ƿ��ߵ��˻���Ԫ�ز���
* @param : ��
* @return: 0����û��⵽��ǵ� 1����⵽����߽ǵ� 2����ǵ���ʧ
* @date  : 2023.5.26
* @author: ������
* @notice: �Ƕ�./180.*PI����Ϊ��õ������ǻ����ƵĲ��ǽǶ���
************************************************/
uint8 CircleIslandRDetection()//����󻷵�
{
    track_type=kTrackLeft;//Ĭ��Ѱ���
    //�������²��ֵ������
    //��ߴ�����ֵ��ʱ���ǳ���̫�������ڵ����
    if((L>CIRCLE_SIDE_LARGE_ADC_THR || R>CIRCLE_SIDE_LARGE_ADC_THR) && M>CIRCLE_MID_ADC_THR)
    {
        return 1;
    }
    return 0;
}

/***********************************************
* @brief : ����Ƿ�����Ҫ�뻷��״̬
* @param : ��
* @return: �Ƿ��뻷 1���뻷 0���뻷�������뻷
* @date  : 2023.5.26
* @author: ������
************************************************/
uint8 CircleIslandRInDetection(void)
{
    static uint8 status=0;
    if(status==0)
    {
        encoder_dis_flag = 1;//��������������
        status=1;
    }
    else if(status==1)
    {
        if(dis>150)//20cm
        {
            encoder_dis_flag = 0;
            status=0;
            track_type=kTrackRight;
            return 1;
        }
    }
    track_type=kTrackLeft;//����û���뻷ʱ��
    return 0;
}

/***********************************************
* @brief : �һ����뻷״̬
* @param : ��
* @return: �Ƿ��뻷���� 1���뻷���� 0���뻷δ����
* @date  : 2023.5.13
* @author: ������
************************************************/
uint8 CircleIslandRIn()//�뻷״̬
{
    //�������ж��Ƿ������뻷״̬
    uint8 len=0;
    if (per_l_line_count>EDGELINE_LENGTH) len=EDGELINE_LENGTH;
    else                                  len=per_l_line_count;
    if(f_left_line1[len-1].X-f_left_line1[0].X>JUDGE_IN_EDD_THR && left_line[l_line_count-1].X>USE_IMAGE_W-80)
    {
        return 1;//�뻷����
    }
    //���뻷״̬����ƫ���
    if(r_line_count<2)//���90�ж����ߣ�˵��ֻ��������߽�ȥ
    {
        //����ɨ��
        RightLineDetectionAgain('n');
        //������ɨ�������ߵ��������Գ���λ�ý����ж�,Ȼ������߽��д���
        if(right_line[0].X>right_line[r_line_count-1].X)//�ұ߿�����Բ���ڻ���ֻ�ܿ���Բ���⻷
        {
            uint8 y=0;//
            l_line_count=1;
            for (uint8 i=0;i<r_line_count;i++)
            {
                if(right_line[r_line_count-1-i].Y>=y)
                {
                    y=right_line[r_line_count-1-i].Y;
                }
                else
                {
                    left_line[l_line_count]=right_line[r_line_count-i];
                    l_line_count++;
                }
            }
            per_l_line_count=PER_EDGELINE_LENGTH;
            //�Ա��߽���͸��
            EdgeLinePerspective(left_line,l_line_count,per_left_line);
            BlurPoints(per_left_line, l_line_count, f_left_line, LINE_BLUR_KERNEL);
            ResamplePoints(f_left_line, l_line_count, f_left_line1, &per_l_line_count, SAMPLE_DIST*PIXEL_PER_METER);
            track_leftline(f_left_line1, per_l_line_count, center_line_l, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
            track_type=kTrackLeft;
        }
    }
    else
    {
        track_type=kTrackRight;//���û����Ѱ����ߵ�Բ����ȥ
    }
    return 0;
}

/***********************************************
* @brief : �һ���������
* @param : ��
* @return: �Ƿ񵽳���״̬ 1���������� 0�����ڻ���
* @date  : 2023.5.13
* @author: ������
************************************************/
uint8 CircleIslandROutDetection()//�󻷵�����״̬
{
    if (l_line_count<10)//���һ��ʼ�Ͷ���
    {
        //�������ɨ��
        LeftLineDetectionAgain('n');
        per_l_line_count=PER_EDGELINE_LENGTH;
        EdgeLinePerspective(left_line,l_line_count,per_left_line);
        BlurPoints(per_left_line, l_line_count, f_left_line, LINE_BLUR_KERNEL);
        ResamplePoints(f_left_line, l_line_count, f_left_line1, &per_l_line_count, SAMPLE_DIST*PIXEL_PER_METER);
        local_angle_points(f_left_line1,per_l_line_count,l_angle,ANGLE_DIST/SAMPLE_DIST);
        nms_angle(l_angle,per_l_line_count,l_angle_1,(ANGLE_DIST/SAMPLE_DIST)*2+1);
        track_leftline(f_left_line1, per_l_line_count, center_line_l, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
    }
    //���нǵ��ж�
    for (int i = 0; i < per_l_line_count; ++i)
    {
        if (70./180.*PI<-l_angle_1[i] && -l_angle_1[i]<140./180.*PI && i<0.8/SAMPLE_DIST)//�����ұ߽ǵ�
        {
            return 1;
        }
    }
    return 0;
}

/***********************************************
* @brief : �һ�������
* @param : ��
* @return: 0������δ���� 1����������
* @date  : 2023.5.13
* @author: ������
************************************************/
uint8 CircleIslandROutFinish(void)//��⻷���Ƿ����
{
    //�����Ǽ�����
    if(icm_angle_x_flag==1)
    {

        return 1;
    }
    return 0;
}
void CircleIslandROut(void)//��������������
{
    //��������
    if (r_line_count>3)//�ұ�������ܿ����߾�����Բ��ȥ
    {
        track_type=kTrackRight;
    }
    else
    {
        if (l_line_count>4)
        {
            //�жϽǵ��λ��
            for (int i = 0; i < per_l_line_count; ++i)
            {
                if (70./180.*PI<-l_angle_1[i] && -l_angle_1[i]<140./180.*PI)//������߽ǵ�
                {
                    if (i>aim_distance/SAMPLE_DIST)
                    {
                        per_l_line_count=i;
                        track_type=kTrackLeft;
                    }
                    else//�̳��ϴε�ƫ��
                    {
                        if (image_bias<-5) image_bias=-8;//����ϴ�̫С�͹̶�����
                        track_type=kTrackSpecial;
                    }
                    break;
                }
            }
        }
        else//���ұ��߶�����
        {
            l_line_count=0;//���¹滮�����
            //�ҵ����ߵ����Ͻǵ�
            uint8 half=GRAY_BLOCK/2;
            myPoint right_seed=right_line[r_line_count-1];//������
            if (right_seed.Y==0)
            {
                right_seed.Y=USE_IMAGE_H_MAX-half-1,right_seed.X=USE_IMAGE_W-half-2;
            }
            for (; right_seed.Y>USE_IMAGE_H_MIN; right_seed.Y--)
            {
                if(PointSobelTest(right_seed)==1) break;
            }
            myPoint left_seed;//������
            left_seed.Y=USE_IMAGE_H-half-1,left_seed.X=half;//ͼ�����½�
            left_line[l_line_count]=left_seed;l_line_count++;//˳���ܴ���Ȼ�����ߵķ���ͷ���
            left_line[l_line_count]=right_seed;l_line_count++;
            //*****************************
            per_l_line_count=PER_EDGELINE_LENGTH;
            EdgeLinePerspective(left_line,l_line_count,per_left_line);
            ResamplePoints(per_left_line, l_line_count, f_left_line1, &per_l_line_count, SAMPLE_DIST*PIXEL_PER_METER);
            track_leftline(f_left_line1, per_l_line_count, center_line_l, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
            track_type=kTrackLeft;
        }
    }
}

/***********************************************
* @brief : �ж��һ����Ƿ����
* @param : ��
* @return: ��
* @date  : 2023.5.13
* @author: ������
************************************************/
uint8 CircleIslandREnd(void)
{
    if(r_line_count>10)//�ж��Ƿ������˻���
    {
        if(right_line[0].X-right_line[10].X>5)//�����ұ�������������������ƣ����������λ������ԶС
            return 1;
    }
    //ѭ��
    track_type=kTrackLeft;//Ѱ���߳�ȥ����
    return 0;
}
/**********************************************************************************************************************/

/***********************************************
* @brief : �Ӷ����ҵ��������ټ�¼���ߣ����������ɨ��
* @param : char choose:ѡ���Ƿ�������ɨ�ߴ�ߵļ�⣬'y': ���� 'n': ������
* @return: ��
* @date  : 2023.5.25
* @author: ������
************************************************/
void LeftLineDetectionAgain(char choose)
{
    uint8 half=GRAY_BLOCK/2;
    myPoint left_seed=left_line[l_line_count-1];//������
    if (left_seed.Y<USE_IMAGE_H_MIN+half || left_seed.Y>USE_IMAGE_H-half-1 || left_seed.X<half || left_seed.X>USE_IMAGE_W-half-1)
    {
        left_seed.Y=USE_IMAGE_H_MAX-half-1;left_seed.X=half;
    }
    left_seed.X++;
    for (; left_seed.Y > 45; left_seed.Y--)
    {
        if (PointSobelTest(left_seed) == 1)
        {
            break;
        }
    }
    l_line_count=0;//����֮����������֮ǰɨ�ߵĴ�������
    uint8 seed_grown_result=0;//���������Ľ��
    uint8 len=EDGELINE_LENGTH-(USE_IMAGE_H_MAX-left_seed.Y);//����ɨ�ߵĳ���
    while(l_line_count<len)
    {
        seed_grown_result=EightAreasSeedGrownGray(&left_seed,'l',&left_seed_num);
        if(seed_grown_result==1)
        {
            left_line[l_line_count]=left_seed;l_line_count++;
            l_growth_direction[(left_seed_num+2)%8]++;
        }
        else break;
    }
    if ((l_growth_direction[1]+l_growth_direction[2])<(l_growth_direction[7]+l_growth_direction[6]) && choose=='y')//�����������������������Ʊ�����ҪС˵������ɨ���Ǵ����
    {
        l_line_count=0;
    }
}

/***********************************************
* @brief : �Ӷ����ҵ��������ټ�¼���ߣ��ұ�������ɨ��
* @param : char choose:ѡ���Ƿ�������ɨ�ߴ�ߵļ�⣬'y': ���� 'n': ������
* @return: ��
* @date  : 2023.5.25
* @author: ������
************************************************/
void RightLineDetectionAgain(char choose)
{
    uint8 half=GRAY_BLOCK/2;
    myPoint right_seed=right_line[r_line_count-1];//������
    if (right_seed.Y<USE_IMAGE_H_MIN+half || right_seed.Y>USE_IMAGE_H-half-1 || right_seed.X<half || right_seed.X>USE_IMAGE_W-half-1)
    {
        right_seed.Y=USE_IMAGE_H_MAX-half-1,right_seed.X=USE_IMAGE_W-half-1;
    }
    right_seed.X--;
    for (; right_seed.Y > 45; right_seed.Y--)
    {
        if (PointSobelTest(right_seed) == 1)
        {
            break;
        }
    }
    r_line_count=0;//����֮����������֮ǰɨ�ߵĴ�������
    uint8 seed_grown_result=0;//���������Ľ��
    uint8 len=EDGELINE_LENGTH-(USE_IMAGE_H_MAX-right_seed.Y);//����ɨ�ߵĳ���
    while(r_line_count<len)
    {
        seed_grown_result=EightAreasSeedGrownGray(&right_seed,'r',&right_seed_num);
        if(seed_grown_result==1)
        {
            right_line[r_line_count]=right_seed;r_line_count++;
            r_growth_direction[(right_seed_num+2)%8]++;
        }
        else break;
    }
    if ((r_growth_direction[1]+r_growth_direction[2])<(r_growth_direction[7]+r_growth_direction[6]) && choose=='y')//�����������������������Ʊ�����ҪС˵������ɨ���Ǵ����
    {
        r_line_count=0;
    }
}

