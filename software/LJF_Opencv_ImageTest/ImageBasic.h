#ifndef _IMAGE_BASIC_H
#define _IMAGE_BASIC_H

#include "main.h"
#include "ImageConversion.h"//��ֵ��ͼ������Լ��궨��

#define EDGELINE_LENGTH USE_IMAGE_H//�������鳤��
#define LOST_LINE_TURE   1//����
#define LOST_LINE_FALSE  0//û����
#define GRAY_BLOCK   7//�Ҷ�ɨ�ߵ�ģ���СBLOCK*BLOCK
#define CLIP_VALUE   3//�Ҷ�ɨ�ߵĲ�����1~5��

/**************ȫ�ֱ���*****************/
extern myPoint left_line[EDGELINE_LENGTH],right_line[EDGELINE_LENGTH];//����������
extern char l_lost_line[EDGELINE_LENGTH],r_lost_line[EDGELINE_LENGTH];
extern uint8 l_line_count,r_line_count;//�����ұ��߼�¼�ܹ��ж೤
extern uint8 l_lostline_num, r_lostline_num;//���Ҷ�����
extern uint8 left_seed_num,right_seed_num;//������������������
extern uint8 l_growth_direction[8],r_growth_direction[8];//��������������
//****************************************

uint8 PointSobelTest(myPoint a);//���ص��sobel����
uint8 InflectionTest(myPoint_f inflection,uint8 x1,uint8 y1,uint8 x2,uint8 y2);//�յ���ٴ��жϣ��������ںڽ�������ɵĴ���յ�
void SowSeedBinary(myPoint* left_seed, myPoint* right_seed);//ɨ��ǰ�Ĳ��ֲ���
void SowSeedGray(uint8 half, char dif_thres, myPoint *left_seed, myPoint *right_seed);//ͨ����Ⱥ��㷨���ҵ���������
uint8 EightAreasSeedGrownGray(myPoint* seed, char choose, uint8 *seed_num);//��������������������������һ��
void EdgeDetection(void);//������ɨ��
void Findline_Lefthand_Adaptive(int block_size, int clip_value,myPoint left_seed,myPoint* left_line, uint8 *num);
void Findline_Righthand_Adaptive(int block_size, int clip_value, myPoint right_seed, myPoint* right_line, uint8 *num);

#endif
