#include "main.h"

#define EDGELINE_LENGTH 200//�������鳤��
#define LOST_LINE_TURE   1//����
#define LOST_LINE_FALSE  0//û����

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
//****************************************

void SowSeed(myPoint* left_seed,myPoint* right_seed);//ɨ��ǰ�Ĳ��ֲ���
uint8 EightAreasSeedGrown(myPoint* seed,char choose,uint8 *seed_num);//��������������������������һ��
void EdgeDetection(void);//������ɨ��
void TrackBasicClear(void);//����������Ϣ�������ã�Ϊ��һ֡��׼��
