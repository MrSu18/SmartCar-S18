#ifndef _IMAGE_CONVERSION_H
#define _IMAGE_CONVERSION_H

#include "zf_device_mt9v03x.h"

//图像转化的宏定义
#define PER_IMAGE_W     188             //逆透视图像的宽度
#define PER_IMAGE_H     120             //逆透视图像的高度
#define USE_IMAGE_W     MT9V03X_W       //使用的图像宽度
#define USE_IMAGE_H     MT9V03X_H       //使用的图像高度
#define USE_IMAGE_H_MIN 45              //使用的图像的远行边界
#define USE_IMAGE_H_MAX 110             //使用的图像的近行边界
#define IMAGE_BLACK     0               //二值化的黑点
#define IMAGE_WHITE     255             //二值化的白点

//宏定义
#define PER_IMG     mt9v03x_image    //用于透视变换的图像
#define IMAGE_BAN   127             //逆透视禁止区域的灰度值

#define use_image   mt9v03x_image   //use_image用于巡线和识别的图像

extern uint8* PerImg_ip[PER_IMAGE_H][PER_IMAGE_W];
extern uint8 left_border[USE_IMAGE_H];//图像左边界
extern uint8 right_border[USE_IMAGE_H];//图像右边界

void ImageBinary(void);//图像二值化


#endif
