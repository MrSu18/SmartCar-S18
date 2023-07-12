#ifndef _IMAGE_CONVERSION_H
#define _IMAGE_CONVERSION_H

#include "zf_device_mt9v03x.h"

//图像转化的宏定义
#define USE_IMAGE_W     MT9V03X_W       //使用的图像宽度
#define USE_IMAGE_H     MT9V03X_H       //使用的图像高度
#define USE_IMAGE_H_MIN 40              //使用的图像的远行边界
#define USE_IMAGE_H_MAX 105             //使用的图像的近行边界
#define IMAGE_BLACK     0               //二值化的黑点
#define IMAGE_WHITE     255             //二值化的白点

//宏定义
#define use_image   gray_image   //use_image用于巡线和识别的图像

extern uint8 binary_image[MT9V03X_H][MT9V03X_W];//二值化图像
extern uint8 gray_image[MT9V03X_H][MT9V03X_W];//使用的缓存灰度图像,避免因为dma把图像冲走
extern uint8 otsu_thr;//大津法对sobel算法之后的图像求得阈值

void sobel(uint8_t imag[MT9V03X_H][MT9V03X_W],uint8_t imag1[MT9V03X_H][MT9V03X_W]);
void myadaptiveThreshold(uint8 *img_data, uint8 *output_data, int width, int height, int block, uint8 clip_value);//上交大自适应二值化
uint8 otsuThreshold(uint8* image, uint16 width, uint16 height);

#endif
