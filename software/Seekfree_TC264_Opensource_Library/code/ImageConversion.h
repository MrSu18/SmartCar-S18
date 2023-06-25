#ifndef _IMAGE_CONVERSION_H
#define _IMAGE_CONVERSION_H

#include "zf_device_mt9v03x.h"

//ͼ��ת���ĺ궨��
#define USE_IMAGE_W     MT9V03X_W       //ʹ�õ�ͼ����
#define USE_IMAGE_H     MT9V03X_H       //ʹ�õ�ͼ��߶�
#define USE_IMAGE_H_MIN 40              //ʹ�õ�ͼ���Զ�б߽�
#define USE_IMAGE_H_MAX 105             //ʹ�õ�ͼ��Ľ��б߽�
#define IMAGE_BLACK     0               //��ֵ���ĺڵ�
#define IMAGE_WHITE     255             //��ֵ���İ׵�

//�궨��
#define use_image   gray_image   //use_image����Ѳ�ߺ�ʶ���ͼ��

extern uint8 binary_image[MT9V03X_H][MT9V03X_W];//��ֵ��ͼ��
extern uint8 gray_image[MT9V03X_H][MT9V03X_W];//ʹ�õĻ���Ҷ�ͼ��,������Ϊdma��ͼ�����
extern uint8 left_border[USE_IMAGE_H];//ͼ����߽�
extern uint8 right_border[USE_IMAGE_H];//ͼ���ұ߽�
extern uint8 otsu_thr;//��򷨶�sobel�㷨֮���ͼ�������ֵ

void ImageBinary(void);//ͼ���ֵ��
void sobel(uint8_t imag[MT9V03X_H][MT9V03X_W],uint8_t imag1[MT9V03X_H][MT9V03X_W]);
uint8 otsuThreshold(uint8* image, uint16 width, uint16 height);

#endif
