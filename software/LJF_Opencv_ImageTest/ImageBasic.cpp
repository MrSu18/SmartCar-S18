#include "ImageBasic.h"
//============================参数================================
#define TRACK_HALF_WIDTH	13//赛道半宽像素点
uint8 const seed_l_flag[8] = { 0,1,2,3,4,5,6,7 };//左种子标号队列数据域
uint8 const seed_r_flag[8] = { 4,3,2,1,0,7,6,5 };//右种子标号队列数据域
//===============================================================

//=========================赛道特征变量=============================
myPoint left_line[EDGELINE_LENGTH], center_line[EDGELINE_LENGTH], right_line[EDGELINE_LENGTH];//左中右三线
uint8 l_line_count=0,c_line_count=0,r_line_count=0;//左中右边线记录总共有多长
uint8 l_lostline_num = 0, r_lostline_num = 0;//左右丢线数
uint8 black_block_num = 0;//赛道黑块个数
uint8 track_top_row = USE_IMAGE_H;//赛道的最顶行，也可起到最长白列的效果
//================================================================
uint8 left_line_x[USE_IMAGE_H], center_line_x[USE_IMAGE_H], right_line_x[USE_IMAGE_H];//左中右三线
/***********************************************
* @brief : 扫线的播种函数，得到最下一行左右线的种子
* @param : 二值化图像
* @return: 左种子、右种子
* @date  : 2022.9.7
* @author: 刘骏帆
************************************************/
#define BLACK_CONTINUE_WIDTH_THR	7//从中间往两边的黑色连续的阈值
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
                left_line[l_line_count].X=left_seed->X;left_line[l_line_count].Y=left_seed->Y;
                l_line_count++;
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
                right_line[r_line_count].X=right_seed->X;right_line[r_line_count].Y=right_seed->Y;
                r_line_count++;
				break;
			}
			else continue;
		}
	}
}

/***********************************************
* @brief : 八零域种子生长的规则，生长一次
* @param : myPoint* seed:要进行生长的种子
*		   SeedGrowAqueue* Aqueue：存储生长标号的循环队列
*		   uint8* queue_data:循环队列的数据域
* @return: 0：生长失败 1：生长成功
* @date  : 2022.9.14
* @author: 刘骏帆
************************************************/
uint8 EightAreasSeedGrown(myPoint* seed,SeedGrowAqueue* seed_queue,const uint8* queue_data)
{
	uint8 break_for_flag = 0;//用于判断是否找到黑色的区域从而不继续标号查看生长
	for (; seed_queue->front != seed_queue->rear; seed_queue->front = (seed_queue->front + 1) % 8)
	{
		switch (queue_data[seed_queue->front])
		{
		case 0:
			if (seed->X+1<=USE_IMAGE_W-1)//防止种子生长越界
			{
				if (use_image[seed->Y][seed->X + 1] == IMAGE_BLACK)
				{
					seed->X++;
					break_for_flag = 1;
				}
			}
			break;
		case 1:
			if (seed->Y-1>=0 && seed->X+1<=USE_IMAGE_W-1)
			{
				if (use_image[seed->Y - 1][seed->X + 1] == IMAGE_BLACK)
				{
					seed->X++; seed->Y--;
					break_for_flag = 1;
				}
			}
			break;
		case 2:
			if (seed->Y-1>=0)
			{
				if (use_image[seed->Y - 1][seed->X] == IMAGE_BLACK)
				{
					seed->Y--;
					break_for_flag = 1;
				}
			}
			break;
		case 3:
			if (seed->Y-1>=0 && seed->X-1>=0)
			{
				if (use_image[seed->Y - 1][seed->X - 1] == IMAGE_BLACK)
				{
					seed->X--; seed->Y--;
					break_for_flag = 1;
				}
			}
			break;
		case 4:
			if(seed->X-1>=0)
			{
				if (use_image[seed->Y][seed->X - 1] == IMAGE_BLACK)
				{
					seed->X--;
					break_for_flag = 1;
				}
			}
			break;
		case 5:
			if (seed->Y+1<=USE_IMAGE_H-1 && seed->X-1>=0)
			{
				if (use_image[seed->Y + 1][seed->X - 1] == IMAGE_BLACK)
				{
					seed->X--; seed->Y++;
					break_for_flag = 1;
				}
			}
			break;
		case 6:
			if (seed->Y+1<=USE_IMAGE_H-1)
			{
				if (use_image[seed->Y + 1][seed->X] == IMAGE_BLACK)
				{
					seed->Y++;
					break_for_flag = 1;
				}
			}
			break;
		case 7:
			if (seed->Y+1<=USE_IMAGE_H-1 && seed->X+1<=USE_IMAGE_W-1)
			{
				if (use_image[seed->Y + 1][seed->X + 1] == IMAGE_BLACK)
				{
					seed->X++; seed->Y++;
					break_for_flag = 1;
				}
			}
			break;
		default:
			break;
		}
		//在这里判断一下前面的switch有没有找到黑色区域
		if (break_for_flag == 1)
		{
			//找到黑点的标号-2作为下一次种子开始的标号
			char temp = seed_queue->front;
			if (temp - 2 < 0)//判断是否-2会小于0，会的话要特殊处理
			{
				seed_queue->front = 6+temp;
			}
			else
			{
				seed_queue->front = temp - 2;
			}
			seed_queue->rear = (seed_queue->front + 7) % 8;//重置队尾
			break;//跳出这次的8领域寻找，进入下一个种子8领域搜索
		}
	}
	if (seed_queue->front == seed_queue->rear)//判断是否八个邻域内都没有黑点
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/***********************************************
* @brief : 八领域扫线函数
* @param : 赛道二值化图像
* @return: 左右边线、左右丢线数
* @date  : 2022.9.16
* @author: 刘骏帆
* @note	 :左种子 3 2 1  右种子 1 2 3
*				4 S 0		 0 S 4
*				5 6 7	     7 6 5
************************************************/
void EdgeDetection(void)
{
    /*1.播种找到左右种子*/
	myPoint left_seed, right_seed;//左右线的初始种子
	left_seed.X = left_border[USE_IMAGE_H - 2]; left_seed.Y = USE_IMAGE_H - 2; right_seed.X = right_border[USE_IMAGE_H - 2]; right_seed.Y = USE_IMAGE_H - 2;//初始化为0
	SowSeed(&left_seed,&right_seed);
	//判断种子是否丢线
	if (left_seed.X != left_border[left_seed.Y])
	{
		black_block_num++;
	}
	if (right_seed.X != right_border[right_seed.Y])
	{
		black_block_num++;
	}
	/*2.种子从左下角开始生长，生长出一个闭合赛道边缘*/
    SeedGrowAqueue l_seed_queue,r_seed_queue;//左右线用于标号转移的循环队列
    l_seed_queue.front = 0; l_seed_queue.rear = 7; r_seed_queue.front = 0; r_seed_queue.rear = 7;
    uint8 change_lr_flag=0,left_finish=0,right_finish=0;//change_lr_flag=0:左边生长 change_lr_flag=1:右边生长
	do
	{
        if(change_lr_flag == 0 && left_finish==0)
        {
            if (EightAreasSeedGrown(&left_seed, &l_seed_queue, seed_l_flag) == 1)//生长一次
            {
                LCDDrawPoint(left_seed.Y, left_seed.X);
                if(left_seed.Y==0 || left_seed.X==right_border[left_seed.Y])//左种子生长到了图像上边界或右边界说明扫完了左边
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
                break;//种子生长失败了跳出循环,避免卡死
            }
        }
        else if(change_lr_flag==1 && right_finish==0)
        {
            if (EightAreasSeedGrown(&right_seed, &r_seed_queue, seed_r_flag) == 1)
            {
                LCDDrawPoint(right_seed.Y, right_seed.X);
                if(right_seed.Y==0 || right_seed.X==left_border[right_seed.Y])//左种子生长到了图像上边界，左边界
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
                break;//种子生长失败了跳出循环,避免卡死
            }
        }
        else break;
	} while (left_seed.Y!=right_seed.Y || left_seed.X != right_seed.X);//当左种子和右种子合并即扫线结束
}

/***********************************************
* @brief :得到图像中赛道分开的黑块区域个数用于初步判定元素 
* @param : 二值化图像，左右边线
* @return: 黑块数
* @date  : 2022.9.16
* @author: 刘骏帆
************************************************/
void CaculateBlackBlock()
{
	//判断扫线的封顶行
	if (track_top_row <= 1)
	{
		for (uint8 column = left_line_x[1]; column < right_line_x[1]; column++)
		{
			if (column == left_border[1] || column == right_border[1])
			{
				continue;
			}
			if ((use_image[1][column] == IMAGE_BLACK && use_image[1][column + 1] == IMAGE_WHITE) || (use_image[1][column] == IMAGE_WHITE && use_image[1][column + 1] == IMAGE_BLACK))
			{
				black_block_num++;
			}
		}
	}
	uint8 l_flag=3, r_flag=3;//避免因为连续的判断一种状态（因为一种状态是不会连续出现的）,因为一开始不知道是丢还是不丢，先给一个状态之外的值
	for (uint8 row = USE_IMAGE_H-5; row > track_top_row; row--)
	{
		//从不丢线到丢线
		if (left_line_x[row] == left_border[row] && left_line_x[row - 2] == left_border[row - 2] && left_line_x[row + 2] != left_border[row + 2] && l_flag != 0)
		{
			black_block_num++;
			l_flag = 0;
		}
		if (right_line_x[row] == right_border[row] && right_line_x[row - 2] == right_border[row - 2] && right_line_x[row + 2] != right_border[row + 2] && r_flag != 0)
		{
			black_block_num++;
			r_flag = 0;
		}
		//从丢线到不丢线
		if (left_line_x[row] != left_border[row] && left_line_x[row - 2] != left_border[row - 2] && left_line_x[row + 2] == left_border[row + 2] && l_flag != 1)
		{
			black_block_num++;
			l_flag = 1;
		}
		if (right_line_x[row] != right_border[row] && right_line_x[row - 2] != right_border[row - 2] && right_line_x[row + 2] == right_border[row + 2] && r_flag != 1)
		{
 			black_block_num++;
			r_flag = 1;
		}
		//顺便在这个操作下获得中线
		center_line_x[row] = (left_line_x[row] + right_line_x[row]) / 2;
	}
}
