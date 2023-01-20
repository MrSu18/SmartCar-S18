#ifndef _IMAGE_BASIC_H
#define _IMAGE_BASIC_H

#include "main.h"
#include "ImageConversion.h"//二值化图像变量以及宏定义

#define EDGELINE_LENGTH USE_IMAGE_H//边线数组长度
#define LOST_LINE_TURE   1//丢线
#define LOST_LINE_FALSE  0//没丢线

typedef struct myPoint
{
	uint8 X;
	uint8 Y;
}myPoint;//点的结构体

/**************全局变量*****************/
extern myPoint left_line[EDGELINE_LENGTH],right_line[EDGELINE_LENGTH];//左中右三线
extern char l_lost_line[EDGELINE_LENGTH],r_lost_line[EDGELINE_LENGTH];
extern uint8 l_line_count,r_line_count;//左中右边线记录总共有多长
extern uint8 l_lostline_num, r_lostline_num;//左右丢线数
//****************************************

void SowSeed(myPoint* left_seed,myPoint* right_seed);//扫线前的播种操作
uint8 EightAreasSeedGrown(myPoint* seed,char choose,uint8 *seed_num);//八零域种子生长规则：种子生长一次
void EdgeDetection(void);//八零域扫线
void TrackBasicClear(void);//赛道基础信息变量重置，为下一帧做准备

#endif
