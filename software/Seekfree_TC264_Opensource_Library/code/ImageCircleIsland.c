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

#define PI 3.1415926

uint8 CircleIslandLStatus()//�ұ߻���״̬״̬��
{
    static uint8 status=0;//����״̬ת�Ʊ���
//    tft180_show_uint(120, 100, status, 2);
    switch (status)
    {
        case 0: //����󻷵�
            if(CircleIslandLDetection()==2)
            {
                status=1;//��Ĭ�ϵ�ż�⵽�Ϳ����뻷����֪��Ч����ô����û����
            }
            break;
        case 1: //·��������һ�����
            if(CircleIslandLInDetection()==1)
            {
                status=2;
            }
            break;
        case 2: //���뻷��
            if(CircleIslandLIn()==1)
            {
                gpio_set_level(P11_12, GPIO_LOW);
                status=3;
            }
            break;
        case 3: //������
            if(CircleIslandLOutDetection()==1)
            {
                track_type=kTrackRight;
                status=4;
            }
            break;
        case 4://����
            if (CircleIslandLOutFinish()==1)
            {
                status=5;
            }
            else CircleIslandLOut();//����û������һֱ����������
            break;
        case 5://��⻷���Ƿ����
            if (CircleIslandLEnd()==1)
            {
                gpio_set_level(P11_12, GPIO_HIGH);
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
* @date  : 2023.3.17
* @author: ������
* @notice: �Ƕ�./180.*PI����Ϊ��õ������ǻ����ƵĲ��ǽǶ���
************************************************/
uint8 CircleIslandLDetection()//����󻷵�
{
    static uint8 status=0;//��⵽�ǵ�->��ⲻ���ǵ�
    if (status==0)//״̬0���ǵ����
    {
        for (int i = 0; i < per_l_line_count; ++i)
        {
            //������70�ȵ�140�ȵĽǵ�,�Ӹ��ŵ���Ϊ��ߵĽǵ�������Ǹ���
            //�������������Ƶ����ĸ��ǵ��ж�Զ0.3m
            if(70./180.*PI<-l_angle_1[i] && -l_angle_1[i]<140./180.*PI && i<0.3/SAMPLE_DIST)
            {
                track_type=kTrackRight;
                status=1;//��һ�μ�⵽
                break;
            }
        }
    }
    else if (status==1)//״̬1���ǵ���ʧ
    {
        track_type=kTrackRight;
        if(l_line_count<2) status=2;
    }
    else if(status==2)
        status=0;//����flag
    return status;
}

/***********************************************
* @brief : ����Ƿ�����Ҫ�뻷��״̬
* @param : ��
* @return: �Ƿ��뻷 1���뻷 0���뻷�������뻷
* @date  : 2023.4.15
* @author: ������
************************************************/
uint8 CircleIslandLInDetection(void)
{
//    static uint8 status=0;
//    if(status==0)
//    {
//        encoder_dis_flag = 1;//��������������
//        status=1;
//    }
//    else if(status==1)
//    {
//        if(dis>400)//40cm
//        {
//            status=2;
//            encoder_dis_flag = 0;
//            status=2;
//        }
//    }
//    else if(status==2)
//    {
        if (adc_value[0]>90)//�ȿ�����Ƿ��б���,����б���ֱ�ӽ�ȥ
        {
            track_type=kTrackLeft;
            return 1;
        }
        else
        {
            track_type=kTrackRight;//����û���뻷ʱ��
            return 0;
        }
//    }
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
    //�����״̬֮���һֱ��������ߵ�����ɨ�ߣ�ָ��ɨ�������߽�ſ�ʼ��¼���飩
    //ͬ���ģ������һ�����Y��ͼ����Ϸ���˵����ʱ��ĳ�����б�ģ����������״̬

    //�������ж��Ƿ������뻷״̬
    int len=0;
    if (per_r_line_count>EDGELINE_LENGTH-10) len=EDGELINE_LENGTH-10;
    else                                  len=per_r_line_count;
    if(f_right_line1[0].X-f_right_line1[len-1].X>JUDGE_IN_EDD_THR && right_line[r_line_count-1].X<80)
    {
        return 1;//�뻷����
    }
    //���뻷״̬����ƫ���
    if(l_line_count<5)//���90�ж����ߣ�˵��ֻ��������߽�ȥ
    {
        //����ɨ��
        LeftLineDetectionAgain();
        //������ɨ�������ߵ��������Գ���λ�ý����ж�,Ȼ������߽��д���
        if(left_line[0].X<left_line[l_line_count-1].X && left_line[0].Y<100)//��߿�����Բ���ڻ���ֻ�ܿ���Բ���⻷
        {
            uint8 y=0;//flag����������
            r_line_count=1;
            for (uint8 i=0;i<l_line_count;i++)
            {
                if(left_line[l_line_count-1-i].Y>=y)
                {
                    y=left_line[l_line_count-1-i].Y;
                }
                else if (left_line[l_line_count-1-i].Y>60)//��Ϊ�Ҷ�ɨ����Զ�����Ҵ�ת���߸��ң�������������ʱ��ô�ã�����Ҫ�޸�
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
        RightLineDetectionAgain();
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
#define TRACK_RIGHTLINE_OUT_THR  (aim_distance/SAMPLE_DIST+5)  //Ѱ����߳���
#define OUT_LISLAND_RIGHTADC_THR    80                         //�󻷳������ұߵ��ADC��ֵ
#define OUT_LISLAND_CENTREADC_THR   80                         //�󻷳����м���ADC��ֵ
uint8 CircleIslandLOutFinish(void)//��⻷���Ƿ����
{
    static uint8 status=0;
    if(adc_value[2]>OUT_LISLAND_CENTREADC_THR)//��ż�⵽ֱ�ӳ�������
    {
        return 1;
    }
    //ͼ���ж�
    if (status==0)
    {
        for (int i = 0; i < per_r_line_count; ++i)
        {
            if (70./180.*PI<r_angle_1[i] && r_angle_1[i]<140./180.*PI)//�����ұ߽ǵ�
            {
                return 0;
            }
        }
        status=1;//���ⲽ˵��û�нǵ���
    }
    else if (status==1)//���״̬�Ǽ�⳵ͷ������������
    {
        if(l_line_count<3 && r_line_count<3)
            status=2;
    }
    else if(status==2)//���״̬ҪҪ�ȴ�����ұ߲�������
    {
        if (r_line_count>10)
        {
            status = 0;
            return 1;
        }
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
        return 1;
    //ѭ��
    if(per_r_line_count>aim_distance/SAMPLE_DIST)//�ұ߲����߲�ȥ�ж����߱��ߵĲ�ֵ������ǰ����
    {
        track_type=kTrackRight;//Ѱ���߳�ȥ����
    }
    else//�����ƫ����д����������ڳ�����ʱ������̫�࣬ʹ���ұ��߲����ڶ��̳���һ�ε�ƫ��
    {
        //����ɨ��
        RightLineDetectionAgain();
        EdgeLinePerspective(right_line,r_line_count,per_right_line);
        if(per_r_line_count>aim_distance/SAMPLE_DIST)
        {
            per_r_line_count=PER_EDGELINE_LENGTH;
            BlurPoints(per_right_line, r_line_count, f_right_line, LINE_BLUR_KERNEL);
            ResamplePoints(per_right_line, r_line_count, f_right_line1, &per_r_line_count, SAMPLE_DIST*PIXEL_PER_METER);
            track_rightline(f_right_line1, r_line_count, center_line_r, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
            track_type=kTrackRight;
        }
        else
        {
            track_type=kTrackSpecial;//ƫ����Ϊ0
            image_bias=-0.5;//�ҹ�һ��
        }
    }
    return 0;
}

/***********************************************
* @brief : �Ӷ����ҵ��������ټ�¼���ߣ����������ɨ��
* @param : ��
* @return: ��
* @date  : 2023.4.21
* @author: ������
************************************************/
void LeftLineDetectionAgain()
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
    uint8 flag=0;//�Ӷ��ߵ�������,0:��û�ҵ����߽磬1:�Ѿ��ҵ��߽�
    while(l_line_count<EDGELINE_LENGTH)
    {
        seed_grown_result=EightAreasSeedGrownGray(&left_seed,'l',&left_seed_num);
        gpio_toggle_level(P21_4);
        if(seed_grown_result==1)
        {
            flag=1;
            left_line[l_line_count]=left_seed;l_line_count++;
        }
        else break;
    }
}

/***********************************************
* @brief : �Ӷ����ҵ��������ټ�¼���ߣ��ұ�������ɨ��
* @param : ��
* @return: ��
* @date  : 2023.4.21
* @author: ������
************************************************/
void RightLineDetectionAgain()
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
    uint8 flag=0;//�Ӷ��ߵ�������,0:��û�ҵ����߽磬1:�Ѿ��ҵ��߽�
    while(r_line_count<EDGELINE_LENGTH)
    {
        seed_grown_result=EightAreasSeedGrownGray(&right_seed,'r',&right_seed_num);
        if(seed_grown_result==1)
        {
            flag=1;
            right_line[r_line_count]=right_seed;r_line_count++;
        }
        else if(seed_grown_result==2)
        {
            if(flag==0) continue;
            else        break;
        }
        else break;
    }
}

