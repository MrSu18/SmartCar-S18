#include "ImageBasic.h"
#include "ImageWR.h"
#include "main.h"

//=========================赛道特征变量=============================
//扫线得到的左右边线
myPoint left_line[EDGELINE_LENGTH],right_line[EDGELINE_LENGTH];//左右边线
uint8 l_line_count=0,r_line_count=0;//左右边线记录总共有多长
char l_lost_line[EDGELINE_LENGTH],r_lost_line[EDGELINE_LENGTH];//左右线是否丢线的记录数组
uint8 l_lostline_num = 0, r_lostline_num = 0;//左右丢线数
//================================================================

/***********************************************
* @brief : 扫线的播种函数，得到最下一行左右线的种子
* @param : 二值化图像
* @return: 左种子、右种子
* @date  : 2022.9.7
* @author: 刘骏帆
************************************************/
#define BLACK_CONTINUE_WIDTH_THR	0//从中间往两边的黑色连续的阈值
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
* @brief : 八零域种子生长的规则，生长一次
* @param : myPoint* seed:要进行生长的种子
*		   char choose: 选择左边还是右边的生长标号表歌
*		   uint8 *seed_num: 八零域的标号
* @return: 0：生长失败 1：生长成功
* @date  : 2023.1.7
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
* @brief : 八领域扫线函数
* @param : 赛道二值化图像
* @return: 左右边线、左右丢线数
* @date  : 2023.1.8
* @author: 刘骏帆
* @note	 :左种子 3 2 1  右种子 1 2 3
*				4 S 0		 0 S 4
*				5 6 7	     7 6 5
************************************************/
uint8 left_seed_num=0,right_seed_num=0;//八零域的种子生长标号，设置成全局变量，为了给再次生长的时候找到标号
void EdgeDetection(void)
{
    /*1.播种找到左右种子*/
	myPoint left_seed, right_seed;//左右线的初始种子
	left_seed.X = left_border[USE_IMAGE_H - 2]; left_seed.Y = USE_IMAGE_H - 2; right_seed.X = right_border[USE_IMAGE_H - 2]; right_seed.Y = USE_IMAGE_H - 2;//初始化为0
	SowSeed(&left_seed,&right_seed);
    left_line[l_line_count]=left_seed;l_line_count++;//将这个点存入边线数组中
    right_line[r_line_count]=right_seed;r_line_count++;
    LCDDrawPoint(left_seed.Y,left_seed.X,255,0,0);
    LCDDrawPoint(right_seed.Y,right_seed.X,0,0,255);
	/*2.种子从左下角开始生长，生长出一个闭合赛道边缘*/
    left_seed_num=0,right_seed_num=0;
    uint8 change_lr_flag=0,left_finish=0,right_finish=0;//change_lr_flag=0:左边生长 change_lr_flag=1:右边生长
    do
	{
        if(change_lr_flag == 0 && left_finish==0)
        {
            if (EightAreasSeedGrown(&left_seed,'l',&left_seed_num) == 1)//生长一次
            {
                LCDDrawPoint(left_seed.Y,left_seed.X,0,255,0);
                //得到边界数组
                left_line[l_line_count]=left_seed;l_line_count++;
                //记录丢线
                if(left_seed.X<=left_border[left_seed.Y])
                {
                    l_lost_line[l_line_count]=LOST_LINE_TURE;
                    l_lostline_num++;
                }
                //切换左右巡线的标志变量
                if(left_seed.Y==0 || left_seed.X==right_border[left_seed.Y] || l_line_count>=EDGELINE_LENGTH-1)//左种子生长到了图像上边界或右边界说明扫完了左边
                {
                    change_lr_flag=!change_lr_flag;
                    left_finish=1;
                }
                else if(left_seed.X==left_border[left_seed.Y] )//左种子生长到了左边界
                {
                    if(right_finish!=1)  change_lr_flag=!change_lr_flag;
                }
            }
            else
            {
                change_lr_flag=!change_lr_flag;
                left_finish=1;
            }
        }
        else if(change_lr_flag==1 && right_finish==0)
        {
            if (EightAreasSeedGrown(&right_seed,'r',&right_seed_num) == 1)
            {
                LCDDrawPoint(right_seed.Y,right_seed.X,255,255,0);
                right_line[r_line_count]=right_seed;r_line_count++;
                if(right_seed.X>=right_border[right_seed.Y])
                {
                    r_lost_line[r_line_count]=LOST_LINE_TURE;
                    r_lostline_num++;
                }
                if(right_seed.Y==0 || right_seed.X==left_border[right_seed.Y] || r_line_count>=EDGELINE_LENGTH-1)//种子生长到了图像上边界，左边界
                {
                    change_lr_flag=!change_lr_flag;
                    right_finish=1;
                }
                else if(right_seed.X==right_border[right_seed.Y])//右种子生长到了右边界
                {
                    if(left_finish!=1)  change_lr_flag=!change_lr_flag;
                }
            }
            else
            {
                change_lr_flag=!change_lr_flag;
                right_finish=1;
            }
        }
        else break;
	} while (left_seed.Y!=right_seed.Y || left_seed.X != right_seed.X);//当左种子和右种子合并即扫线结束
}

#define GRAY_DIF_THRES  10//灰度差比和算法的阈值
void SowSeed(uint8 half,char dif_thres,myPoint *left_seed,myPoint *right_seed)//通过差比和算法先找到左右种子
{
    int dif_gray_value;//灰度值差比和的值
    for (left_seed->Y=USE_IMAGE_H-half-2,left_seed->X=USE_IMAGE_W/2; left_seed->X>half; left_seed->X--)
    {
        //灰度差比和=(f(x,y)-f(x-1,y))/(f(x,y)+f(x-1,y))
        dif_gray_value=100*(use_image[left_seed->Y][left_seed->X]-use_image[left_seed->Y][left_seed->X-1])
                       /(use_image[left_seed->Y][left_seed->X]+use_image[left_seed->Y][left_seed->X-1]);
        if(dif_gray_value>dif_thres) break;
    }
    for (right_seed->Y=USE_IMAGE_H-half-2 ,right_seed->X=USE_IMAGE_W/2; right_seed->X<USE_IMAGE_W-half; right_seed->X++)
    {
        //灰度差比和=(f(x,y)-f(x-1,y))/(f(x,y)+f(x-1,y))
        dif_gray_value=100*(use_image[right_seed->Y][right_seed->X]-use_image[right_seed->Y][right_seed->X+1])
                       /(use_image[right_seed->Y][right_seed->X]+use_image[right_seed->Y][right_seed->X+1]);
        if(dif_gray_value>dif_thres) break;
    }
}

const int dir_front[4][2] = {{0,-1},{1,0},{0,1},{-1,0}};
const int dir_frontleft[4][2] = {{-1,-1},{1,-1},{1, 1},{-1,1}};
const int dir_frontright[4][2] = {{1,-1},{1,1},{-1, 1},{-1,-1}};
void Findline_Lefthand_Adaptive(int block_size, int clip_value,myPoint* pts, uint8 *num)
{
    myPoint left_seed,right_seed;
    int half = block_size / 2;

    SowSeed(half,GRAY_DIF_THRES,&left_seed,&right_seed);
    int step = 0, dir = 0, turn = 0,point_num=0;
    while (step < *num && left_border[left_seed.Y]+half < left_seed.X && left_seed.X < right_border[left_seed.Y] - half - 1 && half < left_seed.Y && left_seed.Y < USE_IMAGE_H - half - 1 && turn < 4)
    {
        //计算当前局部方块的阈值
        int local_thres = 0;
        uint8 gray=0;
        for (int dy = -half; dy <= half; dy++)
        {
            for (int dx = -half; dx <= half; dx++)
            {
                gray=use_image[left_seed.Y+dy][left_seed.X+dx];
                if(gray==IMAGE_BAN) continue;
                else
                {
                    local_thres += gray;
                    point_num++;
                }
            }
        }
        local_thres /= point_num;
        //printf("point_num=%d,",point_num);
        local_thres -= clip_value;
        point_num=0;
        //左手迷宫巡线
        int current_value = use_image[left_seed.Y][left_seed.X];
        int front_value = use_image[left_seed.Y+dir_front[dir][1]][left_seed.X+dir_front[dir][0]];
        int frontleft_value = use_image[left_seed.Y+dir_frontleft[dir][1]][left_seed.X+dir_frontleft[dir][0]];
        if (front_value < local_thres)
        {
            dir = (dir + 1) % 4;
            turn++;
        }
        else if (frontleft_value < local_thres)
        {
            left_seed.X += dir_front[dir][0];
            left_seed.Y += dir_front[dir][1];
            pts[step].X = left_seed.X;
            pts[step].Y = left_seed.Y;
            step++;
            turn = 0;
        }
        else
        {
            left_seed.X += dir_frontleft[dir][0];
            left_seed.Y += dir_frontleft[dir][1];
            dir = (dir + 3) % 4;
            pts[step].X = left_seed.X;
            pts[step].Y = left_seed.Y;
            step++;
            turn = 0;
        }
        //printf("local_thres=%d,front_value=%d,frontleft_value=%d,dir=%d\n",local_thres,front_value,frontleft_value,dir);
//        UseImageDataToUseMat();
//        LCDDrawPoint(left_seed.Y,left_seed.X,0,255,0);
    }
    *num = step;
}
