//
// Created by 30516 on 2023/3/10.
//
#include "ImageSpecial.h"
#include "ImageTrack.h"
#include <stdio.h>
#include "ImageWR.h"

uint8 CircleIslandLStatus()//�ұ߻���״̬״̬��
{
    static uint8 status;//����״̬ת�Ʊ���

    switch (status)
    {
        case 0: //����󻷵�
            if(CircleIslandLDetection()==1)
            {
                printf("CircleIslandDetection success\r\n");
                status=1;//��Ĭ�ϵ�ż�⵽�Ϳ����뻷����֪��Ч����ô����û����
                track_type=kTrackRight;
            }
            break;
        case 1: //·��������һ�����
            track_type=kTrackRight;//���״̬Ĭ����Ѱ�ұ�ֱ��
            if(CircleIslandLInDetection()==1)
            {
                status=2;
            }
            break;
        case 2: //���뻷��
            if(CircleIslandLIn()==1)
            {
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
            if (CircleIslandLOut()==1)
            {
                status=5;
            }
            break;
        case 5://��⻷���Ƿ����
            if (CircleIslandLEnd()==1)
            {
                status=0;
                return 1;
            }
            break;
        default:
            break;
    }
    return 0;
}


uint8 CircleIslandLDetection()//����󻷵�
{
    //�õ�ż�⻷��
    uint8 flag=0;
    for (int i = 0; i < l_line_count/2; ++i)
    {
        if(r_angle_1[i]<1.6&&r_angle_1[i]<1.8)
        {
            return 1;//�һ������
        }
    }
    return 0;
}

/***********************************************
* @brief : ����Ƿ�����Ҫ�뻷��״̬
* @param : ��
* @return: �Ƿ��뻷 1���뻷 0���뻷�������뻷
* @date  : 2023.3.17
* @author: ������
************************************************/
uint8 CircleIslandLInDetection(void)
{
    if(l_line_count<2)//������һ��ʼ�Ͷ��߾��ٴ�ɨ��
    {
        myPoint left_seed=left_line[l_line_count-1];//������
//        uint8 left_seed_num=0;//�����Ӱ�������
        uint8 seed_grown_result=0;//���������Ľ��
        uint8 count=0;//��¼��߶��߶��ٲŵ�������
        while(count<3)
        {
            seed_grown_result=EightAreasSeedGrown(&left_seed,'l',&left_seed_num);
            if(seed_grown_result==1)
            {
                return 1;
            }
            else if(seed_grown_result==2)
            {
                count++;
                continue;
            }
            else break;
        }
        return 0;
    }
    return 0;
}

/***********************************************
* @brief : �󻷵��뻷״̬
* @param : ��
* @return: �Ƿ��뻷���� 1���뻷���� 0���뻷δ����
* @date  : 2023.3.17
* @author: ������
************************************************/
#define JUDGE_IN_EDD_THR    10//�ж��뻷״̬�������ұ�������X����Ĳ�ֵ��ֵ
uint8 CircleIslandLIn()//�뻷״̬
{
    //�����״̬֮���һֱ��������ߵ�����ɨ�ߣ�ָ��ɨ�������߽�ſ�ʼ��¼���飩
    //ͨ���жϵ�һ�����Y�������жϳ���λ�ã���������ֵ��H-4���������׼���뻷
    //ͬ���ģ������һ�����Y��ͼ����Ϸ���˵����ʱ��ĳ�����б�ģ����������״̬

    //�������ж��Ƿ������뻷״̬
    if(right_line[0].X-right_line[r_line_count-1].X>JUDGE_IN_EDD_THR)
    {
        return 1;//�뻷����
    }
    //���뻷״̬����ƫ���
    if(l_line_count<2)
    {
        //����ɨ��
        LeftLineDetectionAgain();
        //������ɨ�������ߵ��������Գ���λ�ý����ж�,Ȼ������߽��д���
        if (left_line[0].Y>USE_IMAGE_H*2/3 && left_line[0].X>left_line[l_line_count-1].X)//�������ٴ�ɨ��ɨ����Բ���ڻ����������߽�ȥ
        {
            //�Ա��߽����˲�
            BlurPoints(left_line, l_line_count, f_left_line, LINE_BLUR_KERNEL);
            //��������
            track_leftline(f_left_line, l_line_count, center_line_l, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
            track_type=kTrackLeft;
        }
        else if(left_line[0].X<left_line[l_line_count-1].X)//��߿�����Բ���ڻ���ֻ�ܿ���Բ���⻷
        {
            uint8 y=0;//
            r_line_count=0;
            for (uint8 i=0;i<l_line_count;i++)
            {
                if(left_line[l_line_count-i].Y>=y)
                {
                    y=left_line[l_line_count-i].Y;
                }
                else
                {
                    right_line[r_line_count]=left_line[l_line_count-i];
                    r_line_count++;
                }
            }
            l_line_count=0;
            BlurPoints(right_line, r_line_count, f_right_line, LINE_BLUR_KERNEL);
            track_rightline(f_right_line, r_line_count, center_line_r, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
            track_type=kTrackRight;
        }
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
* @date  : 2023.3.19
* @author: ������
************************************************/
uint8 CircleIslandLOutDetection()//�󻷵�����״̬
{
    if (r_line_count<2)//�ұ�һ��ʼ�Ͷ���
    {
        //����ɨ��
        RightLineDetectionAgain();
        BlurPoints(right_line, r_line_count, f_right_line, LINE_BLUR_KERNEL);
        local_angle_points(f_right_line,r_line_count,r_angle,ANGLE_DIST/SAMPLE_DIST);
        nms_angle(r_angle,r_line_count,r_angle_1,(ANGLE_DIST/SAMPLE_DIST)*2+1);
        track_rightline(f_right_line, r_line_count, center_line_r, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
    }
    //���нǵ��ж�
    for (int i = 0; i < r_line_count; ++i)
    {
        if (1.5<r_angle_1[i] && r_angle_1[i]<1.8)//�����ұ߽ǵ�
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
* @date  : 2023.3.19
* @author: ������
************************************************/
#define TRACK_RIGHTLINE_OUT_THR  (aim_distance/SAMPLE_DIST+5)  //Ѱ����߳���
uint8 CircleIslandLOut(void)
{
    static uint8 status=0;//�ұ��ߴӲ����ߣ������ߣ��ٵ������ߣ�����������
    //��ѭ��ƫ����д���
    if(r_line_count>TRACK_RIGHTLINE_OUT_THR)
    {
        if (status==0)
        {
            status=1;
        }
        else if(status==2)
        {
            status=0;
            return 1;
        }
        track_type=kTrackRight;
    }
    else//��������̫������Ҫ���߳���
    {
        if (status==1)
        {
            status=2;
        }
        myPoint_f left_inflection={0},right_inflection={0};//���ߵ����ҵ�
        //���ҵ��ҽǵ�
        if (r_line_count<2)//�ұ�һ��ʼ�Ͷ���
        {
            //����ɨ��
            RightLineDetectionAgain();
            BlurPoints(right_line, r_line_count, f_right_line, LINE_BLUR_KERNEL);
            local_angle_points(f_right_line,r_line_count,r_angle,ANGLE_DIST/SAMPLE_DIST);
            nms_angle(r_angle,r_line_count,r_angle_1,(ANGLE_DIST/SAMPLE_DIST)*2+1);
        }
        for (int i = 0; i < r_line_count; ++i)
        {
            if (1.5<r_angle_1[i] && r_angle_1[i]<1.8)//�����ұ߽ǵ�
            {
                r_line_count=i;//�ض�
                right_inflection=f_right_line[i];
                break;
            }
        }
        //����յ�
        if (l_line_count<2 && right_inflection.Y!=0 && right_inflection.X!=0)//������һ��ʼ�Ͷ���,�����ҵ����ҹյ�
        {
            myPoint left_seed=left_line[l_line_count-1];//������
//            uint8 left_seed_num=0;//�����Ӱ�������
            uint8 seed_grown_result=0;//���������Ľ��
            while(seed_grown_result!=1)
            {
                seed_grown_result=EightAreasSeedGrown(&left_seed,'l',&left_seed_num);
            }
            left_inflection.X=(float)left_seed.X;left_inflection.Y=(float)left_seed.Y;
            FillingLine('r',left_inflection,right_inflection);
        }
        //������������֮���ٶ��ұ��߽��д���
        track_rightline(f_right_line, r_line_count, center_line_r, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
        track_type=kTrackRight;
    }
    return 0;
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
    track_type=kTrackRight;//Ѱ���߳�ȥ����
    if(abs(r_line_count-l_line_count)<10)
        return 1;
    else
        return 0;
}

/***********************************************
* @brief : �Ӷ����ҵ��������ټ�¼���ߣ����������ɨ��
* @param : ��
* @return: ��
* @date  : 2023.3.17
* @author: ������
************************************************/
void LeftLineDetectionAgain()
{
    myPoint left_seed=left_line[l_line_count-1];//������
    l_line_count=0;//����֮����������֮ǰɨ�ߵĴ�������
//    uint8 left_seed_num=0;//�����Ӱ�������
    uint8 seed_grown_result=0;//���������Ľ��
    uint8 flag=0;//�Ӷ��ߵ�������,0:��û�ҵ����߽磬1:�Ѿ��ҵ��߽�
    while(l_line_count<EDGELINE_LENGTH)
    {
        seed_grown_result=EightAreasSeedGrown(&left_seed,'l',&left_seed_num);
        if(seed_grown_result==1)
        {
            flag=1;
            left_line[l_line_count]=left_seed;l_line_count++;
        }
        else if(seed_grown_result==2)
        {
            if(flag==0) continue;
            else        break;
        }
        else break;
    }
}

/***********************************************
* @brief : �Ӷ����ҵ��������ټ�¼���ߣ��ұ�������ɨ��
* @param : ��
* @return: ��
* @date  : 2023.3.19
* @author: ������
************************************************/
void RightLineDetectionAgain()
{
    myPoint right_seed=right_line[r_line_count-1];//������
    r_line_count=0;//����֮����������֮ǰɨ�ߵĴ�������
//    uint8 right_seed_num=0;//�����Ӱ�������
    uint8 seed_grown_result=0;//���������Ľ��
    uint8 flag=0;//�Ӷ��ߵ�������,0:��û�ҵ����߽磬1:�Ѿ��ҵ��߽�
    while(r_line_count<EDGELINE_LENGTH)
    {
        seed_grown_result=EightAreasSeedGrown(&right_seed,'r',&right_seed_num);
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
