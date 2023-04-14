#ifndef _IMAGE_CONVERSION_H
#define _IMAGE_CONVERSION_H

#include "zf_device_mt9v03x.h"

//ͼ��ת���ĺ궨��
#define PER_IMAGE_W     188             //��͸��ͼ��Ŀ��
#define PER_IMAGE_H     120             //��͸��ͼ��ĸ߶�
#define USE_IMAGE_W     MT9V03X_W       //ʹ�õ�ͼ����
#define USE_IMAGE_H     MT9V03X_H       //ʹ�õ�ͼ��߶�
#define USE_IMAGE_H_MIN 45              //ʹ�õ�ͼ���Զ�б߽�
#define USE_IMAGE_H_MAX 110             //ʹ�õ�ͼ��Ľ��б߽�
#define IMAGE_BLACK     0               //��ֵ���ĺڵ�
#define IMAGE_WHITE     255             //��ֵ���İ׵�

//�궨��
#define PER_IMG     mt9v03x_image    //����͸�ӱ任��ͼ��
#define IMAGE_BAN   127             //��͸�ӽ�ֹ����ĻҶ�ֵ

#define use_image   mt9v03x_image   //use_image����Ѳ�ߺ�ʶ���ͼ��

extern uint8* PerImg_ip[PER_IMAGE_H][PER_IMAGE_W];
extern uint8 left_border[USE_IMAGE_H];//ͼ����߽�
extern uint8 right_border[USE_IMAGE_H];//ͼ���ұ߽�

void ImageBinary(void);//ͼ���ֵ��


#endif
