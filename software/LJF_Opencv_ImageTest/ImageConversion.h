#ifndef _IMAGE_CONVERSION_H
#define _IMAGE_CONVERSION_H

#include "main.h"

//图像转化的宏定义
#define PER_IMAGE_W     MT9V03X_W             //逆透视图像的宽度
#define PER_IMAGE_H     MT9V03X_H             //逆透视图像的高度
#define USE_IMAGE_W		MT9V03X_W	    //使用的图像宽度
#define USE_IMAGE_H		MT9V03X_H		//使用的图像高度
#define USE_IMAGE_H_MIN 45              //使用的图像的远行边界
#define USE_IMAGE_H_MAX 110             //使用的图像的近行边界
#define IMAGE_BLACK		0				//二值化的黑点
#define IMAGE_WHITE		255				//二值化的白点

#define use_image   mt9v03x_image       //use_image用于巡线和识别的图像

//宏定义
#define PER_IMG     mt9v03x_image   //用于透视变换的图像
#define IMAGE_BAN   255             //逆透视禁止区域的灰度值
#define PERSPECTIVE 1               //透视处理程度选择 0:不对图像逆透视 1:图像逆透视 2:图像逆透视和去畸变

extern uint8* PerImg_ip[PER_IMAGE_H][PER_IMAGE_W];
extern uint8 left_border[USE_IMAGE_H];//图像左边界
extern uint8 right_border[USE_IMAGE_H];//图像右边界
extern uint8 binary_image[MT9V03X_H][MT9V03X_W];


void ImageBinary(void);//图像二值化
void ImagePerspective_Init(void);//图像逆透视地址映射
void ImageBorderInit(void);//逆透视图像边界初始化
void myadaptiveThreshold(uint8 *img_data, uint8 *output_data, int width, int height, int block, uint8 clip_value);
void sobel(uint8 imag[MT9V03X_H][MT9V03X_W],uint8 imag1[MT9V03X_H][MT9V03X_W]);

#endif
