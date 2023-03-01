/*
 * ImageProcess.c
 *
 *  Created on: 2023年3月1日
 *      Author: L
 */
#include "ImageProcess.h"

/***********************************************
* @brief : 出界保护程序，通过遍历图像最后一行的像素点
* @param : void
* @return: void
* @date  : 2023.3.1
* @author: L
************************************************/
void OutProtect(void)
{
    int16 over_count = 0;                                   //存放超过阈值的像素点的数量

    for(int8 i = 0;i < MT9V03X_W;i++)                       //遍历最后一行
    {
        if(mt9v03x_image[MT9V03X_H][i] >= OUT_THRESHOLD)
            over_count++;
    }

    if(over_count >= MT9V03X_W)                             //如果全部超过阈值则停止
        MotorSetPWM(0,0);
}


