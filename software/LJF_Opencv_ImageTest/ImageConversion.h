#include "main.h"

//图像转化的宏定义
#define PER_IMAGE_W     188             //逆透视图像的宽度
#define PER_IMAGE_H     110             //逆透视图像的高度
#define USE_IMAGE_W		MT9V03X_W	    //使用的图像宽度
#define USE_IMAGE_H		MT9V03X_H		//使用的图像高度
#define IMAGE_BLACK		0				//二值化的黑点
#define IMAGE_WHITE		255				//二值化的白点

#define use_image   *PerImg_ip       //use_image用于巡线和识别的图像

extern uint8 binary_image[MT9V03X_H][MT9V03X_W];//二值化后的黑白图变量
extern uint8* PerImg_ip[PER_IMAGE_H][PER_IMAGE_W];
extern uint8 left_border[USE_IMAGE_H];//图像左边界
extern uint8 right_border[USE_IMAGE_H];//图像右边界

void ImageBinary(void);//图像二值化
void ImagePerspective_Init(void);//图像逆透视地址映射
void ImageBorderInit(void);//逆透视图像边界初始化
void BlackBorder(void);//把图像边界都涂黑，初始化使用一次即可
