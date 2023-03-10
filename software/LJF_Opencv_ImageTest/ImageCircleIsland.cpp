//
// Created by 30516 on 2023/3/10.
//
#include "ImageSpecial.h"
#include "ImageTrack.h"
#include <stdio.h>

uint8 CircleIslandRStatus()//�ұ߻���״̬״̬��
{
    static uint8 status;//����״̬ת�Ʊ���

    switch (status)
    {
        case 0: //��⻷��
            if(CircleIslandDetection()==1)
            {
                printf("CircleIslandDetection success\r\n");
                status=1;
            }
            break;
        case 1: //·��������һ�����
            break;
        case 2: //���뻷��
            break;
        case 3: //����
            break;
        default:
            break;
    }
}


uint8 CircleIslandDetection()//����һ���
{
    uint8 flag=0;
    for (int i = 0; i < l_line_count/2; ++i)
    {
        if(r_angle_1[i]<1.6&&r_angle_1[i]<1.8)
        {
            return 1;//�һ������
        }
    }
}
