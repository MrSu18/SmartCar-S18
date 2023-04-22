#include <string.h>
#include "ImageBasic.h"
#include "zf_common_headfile.h"
//=========================赛道特征变量=============================
myPoint left_line[EDGELINE_LENGTH]={0},right_line[EDGELINE_LENGTH]={0};//左右边线
char l_lost_line[EDGELINE_LENGTH]={0},r_lost_line[EDGELINE_LENGTH]={0};//左右线是否丢线的记录数组
uint8 l_line_count=0,r_line_count=0;//左右边线记录总共有多长
uint8 l_lostline_num = 0, r_lostline_num = 0;//左右丢线数
//================================================================

/***********************************************
* @brief : 扫线的播种函数，得到最下一行左右线的种子
* @param : 二值化图像
* @return: 左种子、右种子
* @date  : 2022.9.7
* @author: 刘骏帆
************************************************/
#define BLACK_CONTINUE_WIDTH_THR	5//从中间往两边的黑色连续的阈值
void SowSeed(myPoint* left_seed,myPoint* right_seed)
{
	//找到左边种子
	for (uint8 column = USE_IMAGE_W/2; column>left_border[left_seed->Y]+1; column--)
	{
		//找到白跳黑
		if (use_image[left_seed->Y][column] == IMAGE_WHITE && use_image[left_seed->Y][column - 1] == IMAGE_BLACK)
		{
			//加多个判定条件避免赛道中间的黑色错乱的影响
			uint8 l_black_width = 0;
			for (uint8 i = column - 1; i > 0; i--)
			{
				if (use_image[left_seed->Y][i] == IMAGE_BLACK)
					l_black_width++;
				else
					break;//否则遍历到白色就跳出循环
			}
			if (l_black_width >= BLACK_CONTINUE_WIDTH_THR || (column- BLACK_CONTINUE_WIDTH_THR)<left_border[left_seed->Y])
			{
				left_seed->X = column;
				break;
			}
			else continue;
		}
	}
	//找到右边的种子
	for (uint8 column = left_seed->X; column < right_border[right_seed->Y]-1; column++)
	{
		//找到白跳黑
		if (use_image[right_seed->Y][column] == IMAGE_WHITE && use_image[right_seed->Y][column + 1] == IMAGE_BLACK)
		{
			//加多个判定条件避免赛道中间的黑色错乱的影响
			uint8 r_black_width = 0;
			for (uint8 i = column + 1; i < USE_IMAGE_W-1; i++)
			{
				if (use_image[right_seed->Y][i] == IMAGE_BLACK)
					r_black_width++;
				else
					break;//否则遍历到白色就跳出循环
			}
			if (r_black_width >= BLACK_CONTINUE_WIDTH_THR || (column + BLACK_CONTINUE_WIDTH_THR) > right_border[right_seed->Y])
			{
				right_seed->X = column+1;
				break;
			}
			else continue;
		}
	}
}

/***********************************************
* @brief : 通过差比和算法先找到左右种子
* @param : 灰度图像
* @return: 左种子、右种子
* @date  : 2023.4.12
* @author: 刘骏帆
************************************************/
#define GRAY_DIF_THRES  10//灰度差比和算法的阈值
void SowSeedGray(uint8 half, char dif_thres, myPoint *left_seed, myPoint *right_seed)//通过差比和算法先找到左右种子
{
    for (left_seed->Y = USE_IMAGE_H_MAX - half - 1, left_seed->X = USE_IMAGE_W / 2;left_seed->X > half; left_seed->X--)
    {
        if (PointSobelTest(*left_seed) == 1) break;
    }
    if (left_seed->X == half)//没有成功播种
    {
        for (; left_seed->Y > 90; left_seed->Y--)
        {
            if (PointSobelTest(*left_seed) == 1)
            {
                left_seed->X++;
                break;
            }
        }
    }
    for (right_seed->Y = USE_IMAGE_H_MAX - half - 1, right_seed->X = USE_IMAGE_W / 2;right_seed->X < USE_IMAGE_W - half - 1; right_seed->X++)
    {
        if (PointSobelTest(*right_seed) == 1) break;
    }
    if (right_seed->X == USE_IMAGE_W - half - 1)//没有成功播种
    {
        for (; right_seed->Y > 90; right_seed->Y--)
        {
            if (PointSobelTest(*right_seed) == 1)
            {
                right_seed->X--;
                break;
            }
        }
    }
}

/***********************************************
* @brief : sobel梯度检测
* @param : 要计算的sobel的点
* @return: 1：该点是赛道边沿 0：该点不是赛道边缘
* @date  : 2023.4.21
* @author: 刘骏帆
************************************************/
#define SOBEL_THRES 20//sobel梯度阈值大于就是边沿
uint8 PointSobelTest(myPoint a)//像素点的sobel测试
{
    if (a.X<2 || a.X>USE_IMAGE_W-2-1 || a.Y<2 || a.Y>USE_IMAGE_H-2-1) return 0;//越界
    int gx=0,gy=0,sobel_result=0;
    gx=((-  use_image[a.Y-1][a.X-1])+(-2*use_image[a.Y  ][a.X-1])+(-  use_image[a.Y+1][a.X-1])
       +(   use_image[a.Y-1][a.X+1])+( 2*use_image[a.Y  ][a.X+1])+(   use_image[a.Y+1][a.X+1]))/4;
    if(gx<0)    gx=-gx;
    gy=((   use_image[a.Y+1][a.X-1])
       +( 2*use_image[a.Y+1][a.X  ])
       +(   use_image[a.Y+1][a.X+1])
       +(-  use_image[a.Y-1][a.X-1])
       +(-2*use_image[a.Y-1][a.X  ])
       +(-  use_image[a.Y-1][a.X+1]))/4;
    if(gy<0)    gy=-gy;
    sobel_result=(gx+gy)/2;
    if(sobel_result>otsu_thr)    return 1;
    else                         return 0;
}

/***********************************************
* @brief : 八零域种子生长的规则，生长一次(二值化图)
* @param : myPoint* seed:要进行生长的种子
*		   char choose: 选择左边还是右边的生长标号表歌
*		   uint8 *seed_num: 八零域的标号
* @return: 0：生长失败 1：生长成功 2：生长到了丢线区域
* @date  : 2023.3.5
* @author: 刘骏帆
************************************************/
char const eight_area_left[8][2]={{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1},{0,1},{1,1}};
char const eight_area_right[8][2]={{-1,0},{-1,-1},{0,-1},{1,-1},{1,0},{1,1},{0,1},{-1,1}};
uint8 EightAreasSeedGrown(myPoint* seed,char choose,uint8 *seed_num)
{
    uint8 next_value=0;
    char dx=0,dy=0;
    for(uint8 seed_count=0;seed_count<8;seed_count++)
    {
        switch (choose)
        {
            case 'l':
                dx=eight_area_left[*seed_num][0];
                dy=eight_area_left[*seed_num][1];
                break;
            case 'r':
                dx=eight_area_right[*seed_num][0];
                dy=eight_area_right[*seed_num][1];
                break;
            default:break;
        }
        next_value=use_image[seed->Y+dy][seed->X+dx];
        if (next_value==IMAGE_BLACK)
        {
            seed->X += dx;
            seed->Y += dy;
            if (*seed_num-2<0) *seed_num+=6;
            else               *seed_num-=2;
            return 1;
        }
        else if (seed->Y+dy<=0 || seed->Y+dy>=USE_IMAGE_H-1 || seed->X+dx<=left_border[seed->Y+dy] || seed->X+dx>=right_border[seed->Y+dy])//这边的判断是判断种子生长到了图像边缘，但是由于上一次判断了不是黑点，所以就不用再次判断该点是不是白点了
        {
            seed->X += dx;
            seed->Y += dy;
            if (*seed_num-2<0) *seed_num+=6;
            else               *seed_num-=2;
            return 2;
        }
        else
        {
            *seed_num = (*seed_num + 1) % 8;
        }
    }
    return 0;//循环结束还没找到说明八零域找不到了break
}

/***********************************************
* @brief : 八零域种子生长的规则，生长一次(灰度图)
* @param : uint8 half: 模板block/2
*          uint8 clip_value: 调整阈值（一般在1~5）
*          myPoint* seed:要进行生长的种子
*          char choose: 选择左边还是右边的生长标号表歌
*          uint8 *seed_num: 八零域的标号
* @return: 0：生长失败 1：生长成功
* @date  : 2023.4.12
* @author: 刘骏帆
************************************************/
uint8 EightAreasSeedGrownGray(myPoint* seed,char choose,uint8 *seed_num)
{
    uint8 half=GRAY_BLOCK/2;
    //计算当前局部方块的阈值
    int local_thres = 0;
    int point_num=0;
    static uint8 gray=127;
    for (int i = -half; i <= half; i++)
    {
        for (int j = -half; j <= half; j++)
        {
            if(seed->Y+i<USE_IMAGE_H_MIN || seed->Y+i>USE_IMAGE_H-1 || seed->X+j<0 || seed->X+j>USE_IMAGE_W-1)
                continue;
            else
            {
                local_thres += use_image[seed->Y+i][seed->X+j];
                point_num++;
            }

        }
    }
    local_thres += gray;
    local_thres /= (point_num+1);
    local_thres -= CLIP_VALUE;
    char dx=0,dy=0;
    for(uint8 seed_count=0;seed_count<8;seed_count++)
    {
        switch (choose)
        {
            case 'l':
                dx=eight_area_left[*seed_num][0];
                dy=eight_area_left[*seed_num][1];
                break;
            case 'r':
                dx=eight_area_right[*seed_num][0];
                dy=eight_area_right[*seed_num][1];
                break;
            default:break;
        }
        uint8 next_value=use_image[seed->Y+dy][seed->X+dx];
        if (seed->Y+dy<=USE_IMAGE_H_MIN+half || seed->Y+dy>=USE_IMAGE_H-half-1 || seed->X+dx<=half || seed->X+dx>=USE_IMAGE_W-half-1)//这边的判断是判断种子生长到了图像边缘，但是由于上一次判断了不是黑点，所以就不用再次判断该点是不是白点了
        {
            seed->X += dx;
            seed->Y += dy;
            if (*seed_num-2<0) *seed_num+=6;
            else               *seed_num-=2;
            return 2;
        }
        else if (next_value<local_thres)
        {
            gray=next_value;
            seed->X += dx;
            seed->Y += dy;
            if (*seed_num-2<0) *seed_num+=6;
            else               *seed_num-=2;
            return 1;
        }
        else
            *seed_num = (*seed_num + 1) % 8;
    }
    return 0;//循环结束还没找到说明八零域找不到了break
}

/***********************************************
* @brief : 八领域扫线函数
* @param : 赛道灰度图
* @return: 左右边线、左右丢线数
* @date  : 2023.4.21
* @author: 刘骏帆
* @note  :左种子 3 2 1  右种子 1 2 3
*               4 S 0        0 S 4
*               5 6 7        7 6 5
************************************************/
uint8 left_seed_num=0,right_seed_num=0;//八零域的种子生长标号，设置成全局变量，为了给再次生长的时候找到标号
void EdgeDetection(void)
{
    /*播种*/
    myPoint left_seed,right_seed;
    SowSeedGray(GRAY_BLOCK/2, GRAY_DIF_THRES, &left_seed, &right_seed);
    left_line[l_line_count]=left_seed;l_line_count++;
    right_line[r_line_count]=right_seed;r_line_count++;
    /*种子生长*/
    left_seed_num=0,right_seed_num=0;
    //左边线生长
    while(l_line_count<EDGELINE_LENGTH)
    {
        uint8 seed_grow_result=EightAreasSeedGrownGray(&left_seed, 'l', &left_seed_num);
        if (seed_grow_result==1)
        {
            left_line[l_line_count].X=left_seed.X;left_line[l_line_count].Y=left_seed.Y;
            l_line_count++;
        }
        else break;
    }
    //右边线生长
    while(r_line_count<EDGELINE_LENGTH)
    {
        uint8 seed_grow_result= EightAreasSeedGrownGray(&right_seed, 'r', &right_seed_num);
        if (seed_grow_result==1)
        {
            right_line[r_line_count].X=right_seed.X;right_line[r_line_count].Y=right_seed.Y;
            r_line_count++;
        }
        else break;
    }
}

