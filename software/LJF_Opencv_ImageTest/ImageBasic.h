#include "ImageConversion.h"//二值化图像变量以及宏定义

#define EDGELINE_LENGTH 200//边线数组长度

typedef struct myPoint
{
	uint8 X;
	uint8 Y;
}myPoint;//点的结构体
//用于给八邻域种子生长标号起始标号做一个循环队列的操作
//起始的点先看上一次找到黑点的标号-2作为下一次的0号点：循环队列中的队头从上次找到的点-2，队尾为队头+7%8
//遍历8个领域的时候队头++，循环结束的条件为，队头==队尾
typedef struct SeedGrowAqueue
{
	uint8 front;//队头
	uint8 rear;//队尾
}SeedGrowAqueue;//种子生长的队列

/**************全局变量*****************/
extern uint8 left_line_x[USE_IMAGE_H], center_line_x[USE_IMAGE_H], right_line_x[USE_IMAGE_H];//左中右三线
extern uint8 black_block_num;//赛道黑块个数
extern uint8 l_lostline_num, r_lostline_num;//左右丢线数
extern uint8 track_top_row;//赛道的最顶行，也可起到最长白列的效果
//****************************************

void SowSeed(myPoint* left_seed,myPoint* right_seed);//扫线前的播种操作
uint8 EightAreasSeedGrown(myPoint* seed,char choose,uint8 *seed_count,uint8 *seed_num);//八零域种子生长规则：种子生长一次
void EdgeDetection(void);//八零域扫线
void CaculateBlackBlock(void);//计算图像黑色区块
