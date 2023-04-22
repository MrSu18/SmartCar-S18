#ifndef _IMAGE_BASIC_H
#define _IMAGE_BASIC_H

#include "ImageConversion.h"//��ֵ��ͼ������Լ��궨��

#define EDGELINE_LENGTH 120//�������鳤��
#define LOST_LINE_TURE   1//����
#define LOST_LINE_FALSE  0//û����
#define GRAY_BLOCK   7//�Ҷ�ɨ�ߵ�ģ���СBLOCK*BLOCK
#define CLIP_VALUE   2//�Ҷ�ɨ�ߵĲ�����1~5��

typedef struct myPoint
{
	uint8 X;
	uint8 Y;
}myPoint;//��Ľṹ��

/**************ȫ�ֱ���*****************/
extern myPoint left_line[EDGELINE_LENGTH],right_line[EDGELINE_LENGTH];//����������
extern char l_lost_line[EDGELINE_LENGTH],r_lost_line[EDGELINE_LENGTH];
extern uint8 l_line_count,r_line_count;//�����ұ��߼�¼�ܹ��ж೤
extern uint8 l_lostline_num, r_lostline_num;//���Ҷ�����
extern uint8 left_seed_num,right_seed_num;//������������������
//****************************************

uint8 PointSobelTest(myPoint a);//���ص��sobel����
void SowSeed(myPoint* left_seed,myPoint* right_seed);//ɨ��ǰ�Ĳ��ֲ���
uint8 EightAreasSeedGrown(myPoint* seed,char choose,uint8 *seed_num);//��������������������������һ��
uint8 EightAreasSeedGrownGray(myPoint* seed,char choose,uint8 *seed_num);//�Ҷ�ɨ��
void EdgeDetection(void);//������ɨ��

#endif
