#ifndef _IMAGE_CONVERSION_H
#define _IMAGE_CONVERSION_H

#include "main.h"

//ͼ��ת���ĺ궨��
#define PER_IMAGE_W     MT9V03X_W             //��͸��ͼ��Ŀ��
#define PER_IMAGE_H     MT9V03X_H             //��͸��ͼ��ĸ߶�
#define USE_IMAGE_W		MT9V03X_W	    //ʹ�õ�ͼ����
#define USE_IMAGE_H		MT9V03X_H		//ʹ�õ�ͼ��߶�
#define USE_IMAGE_H_MIN 45              //ʹ�õ�ͼ���Զ�б߽�
#define USE_IMAGE_H_MAX 110             //ʹ�õ�ͼ��Ľ��б߽�
#define IMAGE_BLACK		0				//��ֵ���ĺڵ�
#define IMAGE_WHITE		255				//��ֵ���İ׵�

#define use_image   mt9v03x_image       //use_image����Ѳ�ߺ�ʶ���ͼ��

//�궨��
#define PER_IMG     mt9v03x_image   //����͸�ӱ任��ͼ��
#define IMAGE_BAN   255             //��͸�ӽ�ֹ����ĻҶ�ֵ
#define PERSPECTIVE 1               //͸�Ӵ���̶�ѡ�� 0:����ͼ����͸�� 1:ͼ����͸�� 2:ͼ����͸�Ӻ�ȥ����

extern uint8* PerImg_ip[PER_IMAGE_H][PER_IMAGE_W];
extern uint8 left_border[USE_IMAGE_H];//ͼ����߽�
extern uint8 right_border[USE_IMAGE_H];//ͼ���ұ߽�
extern uint8 binary_image[MT9V03X_H][MT9V03X_W];


void ImageBinary(void);//ͼ���ֵ��
void ImagePerspective_Init(void);//ͼ����͸�ӵ�ַӳ��
void ImageBorderInit(void);//��͸��ͼ��߽��ʼ��
void myadaptiveThreshold(uint8 *img_data, uint8 *output_data, int width, int height, int block, uint8 clip_value);
void sobel(uint8 imag[MT9V03X_H][MT9V03X_W],uint8 imag1[MT9V03X_H][MT9V03X_W]);

#endif
