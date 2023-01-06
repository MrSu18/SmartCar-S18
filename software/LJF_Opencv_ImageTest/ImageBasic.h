#include "ImageConversion.h"//��ֵ��ͼ������Լ��궨��

#define EDGELINE_LENGTH 200//�������鳤��

typedef struct myPoint
{
	uint8 X;
	uint8 Y;
}myPoint;//��Ľṹ��
//���ڸ��������������������ʼ�����һ��ѭ�����еĲ���
//��ʼ�ĵ��ȿ���һ���ҵ��ڵ�ı��-2��Ϊ��һ�ε�0�ŵ㣺ѭ�������еĶ�ͷ���ϴ��ҵ��ĵ�-2����βΪ��ͷ+7%8
//����8�������ʱ���ͷ++��ѭ������������Ϊ����ͷ==��β
typedef struct SeedGrowAqueue
{
	uint8 front;//��ͷ
	uint8 rear;//��β
}SeedGrowAqueue;//���������Ķ���

/**************ȫ�ֱ���*****************/
extern uint8 left_line_x[USE_IMAGE_H], center_line_x[USE_IMAGE_H], right_line_x[USE_IMAGE_H];//����������
extern uint8 black_block_num;//�����ڿ����
extern uint8 l_lostline_num, r_lostline_num;//���Ҷ�����
extern uint8 track_top_row;//��������У�Ҳ��������е�Ч��
//****************************************

void SowSeed(myPoint* left_seed,myPoint* right_seed);//ɨ��ǰ�Ĳ��ֲ���
uint8 EightAreasSeedGrown(myPoint* seed,char choose,uint8 *seed_count,uint8 *seed_num);//��������������������������һ��
void EdgeDetection(void);//������ɨ��
void CaculateBlackBlock(void);//����ͼ���ɫ����
