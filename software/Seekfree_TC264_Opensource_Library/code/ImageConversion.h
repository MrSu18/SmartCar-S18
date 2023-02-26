#ifndef _IMAGE_CONVERSION_H
#define _IMAGE_CONVERSION_H

#include "zf_device_mt9v03x.h"

//ͼ��ת���ĺ궨��
#define PER_IMAGE_W     96             //��͸��ͼ��Ŀ��
#define PER_IMAGE_H     60             //��͸��ͼ��ĸ߶�
#define USE_IMAGE_W		PER_IMAGE_W	    //ʹ�õ�ͼ����
#define USE_IMAGE_H		PER_IMAGE_H		//ʹ�õ�ͼ��߶�
#define IMAGE_BLACK		0				//��ֵ���ĺڵ�
#define IMAGE_WHITE		255				//��ֵ���İ׵�

#define use_image   *PerImg_ip       //use_image����Ѳ�ߺ�ʶ���ͼ��

extern uint8* PerImg_ip[PER_IMAGE_H][PER_IMAGE_W];
extern uint8 left_border[USE_IMAGE_H];//ͼ����߽�
extern uint8 right_border[USE_IMAGE_H];//ͼ���ұ߽�

void ImageBinary(void);//ͼ���ֵ��
void ImagePerspective_Init(void);//ͼ����͸�ӵ�ַӳ��
void ImageBorderInit(void);//��͸��ͼ��߽��ʼ��

#endif
