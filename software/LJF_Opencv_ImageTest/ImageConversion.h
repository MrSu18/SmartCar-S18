#include "main.h"

//ͼ��ת���ĺ궨��
#define PER_IMAGE_W     188             //��͸��ͼ��Ŀ��
#define PER_IMAGE_H     110             //��͸��ͼ��ĸ߶�
#define USE_IMAGE_W		MT9V03X_W	    //ʹ�õ�ͼ����
#define USE_IMAGE_H		MT9V03X_H		//ʹ�õ�ͼ��߶�
#define IMAGE_BLACK		0				//��ֵ���ĺڵ�
#define IMAGE_WHITE		255				//��ֵ���İ׵�

#define use_image   *PerImg_ip       //use_image����Ѳ�ߺ�ʶ���ͼ��

extern uint8 binary_image[MT9V03X_H][MT9V03X_W];//��ֵ����ĺڰ�ͼ����
extern uint8* PerImg_ip[PER_IMAGE_H][PER_IMAGE_W];
extern uint8 left_border[USE_IMAGE_H];//ͼ����߽�
extern uint8 right_border[USE_IMAGE_H];//ͼ���ұ߽�

void ImageBinary(void);//ͼ���ֵ��
void ImagePerspective_Init(void);//ͼ����͸�ӵ�ַӳ��
void ImageBorderInit(void);//��͸��ͼ��߽��ʼ��
void BlackBorder(void);//��ͼ��߽綼Ϳ�ڣ���ʼ��ʹ��һ�μ���
