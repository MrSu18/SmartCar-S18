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
                status=1;
                track_type=kTrackRight;
            }
            break;
        case 1: //·��������һ�����
            track_type=kTrackRight;//���״̬Ĭ����Ѱ�ұ�ֱ��
            if(1)
            {
                status=2;
                track_type=kTrackLeft;
            }
            break;
        case 2: //���뻷��
            break;
        case 3: //����
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

uint8 CircleIslandLInDetection()//����Ƿ�����Ҫ�뻷��״̬
{
    //�����״̬֮���һֱ��������ߵ�����ɨ�ߣ�ָ��ɨ�������߽�ſ�ʼ��¼���飩
    //ͨ���жϵ�һ�����Y�������жϳ���λ�ã���������ֵ��H-4���������׼���뻷
    //ͬ���ģ������һ�����Y��ͼ����Ϸ���˵����ʱ��ĳ�����б�ģ����������״̬

    if(l_line_count<5)
    {
        //��������ɨ��
        myPoint left_seed=left_line[l_line_count-1];//������
        l_line_count=0;//����֮����������֮ǰɨ�ߵĴ�������
        uint8 left_seed_num=0;//�����Ӱ�������
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
            r_line_count=0;
            for (uint8 i=0;i<l_line_count;i++)
            {
                right_line[r_line_count]=left_line[l_line_count-i];
                r_line_count++;
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
