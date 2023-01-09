#include "main.h"

#define EDGELINE_LENGTH 200//边线数组长度

typedef struct myPoint
{
	uint8 X;
	uint8 Y;
}myPoint;//点的结构体

/**************全局变量*****************/
extern myPoint left_line[EDGELINE_LENGTH], center_line[EDGELINE_LENGTH], right_line[EDGELINE_LENGTH];//左中右三线
extern uint8 l_line_count,c_line_count,r_line_count;//左中右边线记录总共有多长
extern uint8 l_lostline_num, r_lostline_num;//左右丢线数
//****************************************

void SowSeed(myPoint* left_seed,myPoint* right_seed);//扫线前的播种操作
uint8 EightAreasSeedGrown(myPoint* seed,char choose,uint8 *seed_num);//八零域种子生长规则：种子生长一次
void EdgeDetection(void);//八零域扫线
void TrackBasicClear(void);//赛道基础信息变量重置，为下一帧做准备
