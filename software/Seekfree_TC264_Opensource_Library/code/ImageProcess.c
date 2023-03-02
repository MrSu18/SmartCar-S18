/*
 * ImageProcess.c
 *
 *  Created on: 2023��3��1��
 *      Author: L
 */
#include "ImageProcess.h"

/***********************************************
* @brief : ���籣������ͨ������ͼ�����һ�е����ص�
* @param : void
* @return: void
* @date  : 2023.3.1
* @author: L
************************************************/
void OutProtect(void)
{
    int over_count = 0;                                   //��ų�����ֵ�����ص������

    for(int16 i = 0;i < MT9V03X_W;i++)                       //�������һ��
    {
        for(int16 j = MT9V03X_H;j < MT9V03X_H - 3;j--)
        {
            if(mt9v03x_image[j][i] <= OUT_THRESHOLD)
                over_count++;
        }
    }

    if(over_count >= MT9V03X_W)                             //���ȫ��������ֵ��ֹͣ
        MotorSetPWM(0,0);
}


