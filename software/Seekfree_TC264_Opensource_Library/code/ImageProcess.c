/*
 * ImageProcess.c
 *
 *  Created on: 2023��3��1��
 *      Author: L
 */
#include "ImageProcess.h"
#include "zf_device_tft180.h"
#include "zf_driver_pit.h"

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

    for(int16 i = 0;i < MT9V03X_W;i++)                       //�������һ��
    {
        if(mt9v03x_image[MT9V03X_H-1][i] <= OUT_THRESHOLD)
                over_count++;
    }

    if(over_count >= MT9V03X_W)                             //���ȫ��������ֵ��ֹͣ
    {

        while(1)
        {
            pit_disable(CCU60_CH0);//�رյ���ж�
            MotorSetPWM(0,0);
//            tft180_show_int(0, 0, over_count, 3);
        }
    }

}


