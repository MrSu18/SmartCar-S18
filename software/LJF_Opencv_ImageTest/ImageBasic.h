#ifndef _IMAGE_BASIC_H
#define _IMAGE_BASIC_H

#include "main.h"
#include "ImageConversion.h"//二值化图像变量以及宏定义

#define EDGELINE_LENGTH USE_IMAGE_H//边线数组长度
#define LOST_LINE_TURE   1//丢线
#define LOST_LINE_FALSE  0//没丢线
#define GRAY_BLOCK   7//灰度扫线的模板大小BLOCK*BLOCK
#define CLIP_VALUE   3//灰度扫线的参数（1~5）

/**************全局变量*****************/
extern myPoint left_line[EDGELINE_LENGTH],right_line[EDGELINE_LENGTH];//左中右三线
extern char l_lost_line[EDGELINE_LENGTH],r_lost_line[EDGELINE_LENGTH];
extern uint8 l_line_count,r_line_count;//左中右边线记录总共有多长
extern uint8 l_lostline_num, r_lostline_num;//左右丢线数
extern uint8 left_seed_num,right_seed_num;//八零域的种子生长标号
extern uint8 l_growth_direction[8],r_growth_direction[8];//左右线生长方向
//****************************************

uint8 PointSobelTest(myPoint a);//像素点的sobel测试
uint8 InflectionTest(myPoint_f inflection,uint8 x1,uint8 y1,uint8 x2,uint8 y2);//拐点的再次判断，避免由于黑胶布而造成的错误拐点
void SowSeedBinary(myPoint* left_seed, myPoint* right_seed);//扫线前的播种操作
void SowSeedGray(uint8 half, char dif_thres, myPoint *left_seed, myPoint *right_seed);//通过差比和算法先找到左右种子
uint8 EightAreasSeedGrownGray(myPoint* seed, char choose, uint8 *seed_num);//八零域种子生长规则：种子生长一次
void EdgeDetection(void);//八零域扫线
void Findline_Lefthand_Adaptive(int block_size, int clip_value,myPoint left_seed,myPoint* left_line, uint8 *num);
void Findline_Righthand_Adaptive(int block_size, int clip_value, myPoint right_seed, myPoint* right_line, uint8 *num);

#endif
