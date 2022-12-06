#include "ImageBasic.h"
#include <stdio.h>

uint8 left_line[USE_IMAGE_H], center_line[USE_IMAGE_H], right_line[USE_IMAGE_H];//����������
uint8 left_line_y[USE_IMAGE_W] = { 0 }, right_line_y[USE_IMAGE_W] = {USE_IMAGE_H-1};//�����߽��Y����
uint8 black_block_num = 0;//�����ڿ����
uint8 const seed_l_flag[8] = { 0,1,2,3,4,5,6,7 };//�����ӱ�Ŷ���������
uint8 const seed_r_flag[8] = { 4,3,2,1,0,7,6,5 };//�����ӱ�Ŷ���������
uint8 l_lostline_num = 0, r_lostline_num = 0;//���Ҷ�����
uint8 leftline_endrow = USE_IMAGE_H, rightline_endrow = USE_IMAGE_H;//�����ߵĽ�����

#define BLACK_CONTINUE_WIDTH_THR	10//���м������ߵĺ�ɫ��������ֵ
#define TRACK_HALF_WIDTH	13//����������ص�	

/***********************************************
* @brief : ɨ�ߵĲ��ֺ������õ�����һ�������ߵ�����
* @param : ��ֵ��ͼ��
* @return: �����ӡ�������
* @date  : 2022.9.7
* @author: ������
************************************************/
void SowSeed(myPoint* left_seed,myPoint* right_seed)
{
	//�ҵ��������
	for (uint8 column = USE_IMAGE_W/2; column>left_border[left_seed->Y]+1; column--)
	{
		//�ҵ�������
		if (use_image[left_seed->Y][column] == IMAGE_WHITE && use_image[left_seed->Y][column - 1] == IMAGE_BLACK)
		{
			//�Ӷ���ж��������������м�ĺ�ɫ���ҵ�Ӱ��
			uint8 l_black_width = 0;
			for (uint8 i = column - 1; i > 0; i--)
			{
				if (use_image[left_seed->Y][i] == IMAGE_BLACK)
					l_black_width++;
				else
					break;//�����������ɫ������ѭ��
			}
			if (l_black_width >= BLACK_CONTINUE_WIDTH_THR || (column- BLACK_CONTINUE_WIDTH_THR)<left_border[left_seed->Y])
			{
				left_seed->X = column;
				left_line[left_seed->Y] = left_seed->X;
				break;
			}
			else continue;
		}
	}
	//�ҵ��ұߵ�����
	for (uint8 column = USE_IMAGE_W / 2; column < right_border[right_seed->Y]-1; column++)
	{
		//�ҵ�������
		if (use_image[right_seed->Y][column] == IMAGE_WHITE && use_image[right_seed->Y][column + 1] == IMAGE_BLACK)
		{
			//�Ӷ���ж��������������м�ĺ�ɫ���ҵ�Ӱ��
			uint8 r_black_width = 0;
			for (uint8 i = column + 1; i < USE_IMAGE_W-1; i++)
			{
				if (use_image[right_seed->Y][i] == IMAGE_BLACK)
					r_black_width++;
				else
					break;//�����������ɫ������ѭ��
			}
			if (r_black_width >= BLACK_CONTINUE_WIDTH_THR || (column + BLACK_CONTINUE_WIDTH_THR) > right_border[right_seed->Y])
			{
				right_seed->X = column+1;
				right_line[right_seed->Y] = right_seed->X;
				break;
			}
			else continue;
		}
	}
}

/***********************************************
* @brief : ���������������Ĺ�������һ��
* @param : myPoint* seed:Ҫ��������������
*		   SeedGrowAqueue* Aqueue���洢������ŵ�ѭ������
*		   uint8* queue_data:ѭ�����е�������
* @return: 0������ʧ�� 1�������ɹ�
* @date  : 2022.9.14
* @author: ������
************************************************/
uint8 EightAreasSeedGrown(myPoint* seed,SeedGrowAqueue* seed_queue,const uint8* queue_data)
{
	uint8 break_for_flag = 0;//�����ж��Ƿ��ҵ���ɫ������Ӷ���������Ų鿴����
	for (; seed_queue->front != seed_queue->rear; seed_queue->front = (seed_queue->front + 1) % 8)
	{
		switch (queue_data[seed_queue->front])
		{
		case 0:
			if (seed->X+1<=right_border[seed->Y])//��ֹ��������Խ��
			{
				if (use_image[seed->Y][seed->X + 1] == IMAGE_BLACK)
				{
					seed->X++;
					break_for_flag = 1;
				}
			}
			break;
		case 1:
			if (seed->Y-1>=0 && seed->X+1<=right_border[seed->Y-1])
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
			if (seed->Y+1<=USE_IMAGE_H-1 && seed->X+1<=right_border[seed->Y+1])
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
		//�������ж�һ��ǰ���switch��û���ҵ���ɫ����
		if (break_for_flag == 1)
		{
			//�ҵ��ڵ�ı��-2��Ϊ��һ�����ӿ�ʼ�ı��
			char temp = seed_queue->front;
			if (temp - 2 < 0)//�ж��Ƿ�-2��С��0����Ļ�Ҫ���⴦��
			{
				seed_queue->front = 6+temp;
			}
			else
			{
				seed_queue->front = temp - 2;
			}
			seed_queue->rear = (seed_queue->front + 7) % 8;//���ö�β
			break;//������ε�8����Ѱ�ң�������һ������8��������
		}
	}
	if (seed_queue->front == seed_queue->rear)//�ж��Ƿ�˸������ڶ�û�кڵ�
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/***********************************************
* @brief : ������ɨ�ߺ���
* @param : ������ֵ��ͼ��
* @return: ���ұ��ߡ����Ҷ�����
* @date  : 2022.9.16
* @author: ������
* @note	 :������ 3 2 1  ������ 1 2 3
*				4 S 0		 0 S 4
*				5 6 7	     7 6 5
************************************************/
void EdgeDetection(void)
{
	myPoint left_seed, right_seed;//�����ߵĳ�ʼ����
	left_seed.X = left_border[USE_IMAGE_H - 2]; left_seed.Y = USE_IMAGE_H - 2; right_seed.X = right_border[USE_IMAGE_H - 2]; right_seed.Y = USE_IMAGE_H - 2;//��ʼ��Ϊ0
	SeedGrowAqueue l_seed_queue,r_seed_queue;//���������ڱ��ת�Ƶ�ѭ������
	l_seed_queue.front = 0; l_seed_queue.rear = 7; r_seed_queue.front = 0; r_seed_queue.rear = 7;
	/*1.�����ҵ���������*/
	SowSeed(&left_seed,&right_seed);
	//�ж������Ƿ���
	if (left_seed.X != left_border[left_seed.Y])
	{
		black_block_num++;
	}
	if (right_seed.X != right_border[right_seed.Y])
	{
		black_block_num++;
	}
	/*2.���Ӵ����½ǿ�ʼ������������һ���պ�������Ե*/
	uint8 seed_grow_top_flag=0;//��־�������Ƿ���������ͼ�񶥶���Ҫ�л������ߵ��жϣ�0������ߣ�1���ұ���
	do
	{
		LCDDrawPoint(left_seed.Y, left_seed.X);
		if (EightAreasSeedGrown(&left_seed, &l_seed_queue, &seed_l_flag[0]) == 1)
		{
			//��¼��ߵ����У��������н��м�¼
			if (left_seed.Y < leftline_endrow)
			{
				leftline_endrow = left_seed.Y;
				//�ж������Ƿ���������ͼ�񶥶�
				if (leftline_endrow == 0)	seed_grow_top_flag = 1;
			}
			//�ж��Ƿ��Ǳ���,�������н��м�¼
			if (seed_grow_top_flag == 0)//����������߽�
			{
				if (use_image[left_seed.Y][left_seed.X + 1] == IMAGE_WHITE && use_image[left_seed.Y][left_seed.X + 2] == IMAGE_WHITE)
				{
					if (left_seed.X > left_line[left_seed.Y])
					{
						left_line[left_seed.Y] = left_seed.X;
					}
				}
				if (left_seed.X == left_border[left_seed.Y])//�ж��Ƿ���
				{
					l_lostline_num++;
				}
			}
			else//�����������ұ߽���ж�
			{
				if (use_image[left_seed.Y][left_seed.X - 1] == IMAGE_WHITE && use_image[left_seed.Y][left_seed.X - 2] == IMAGE_WHITE)
				{
					if (left_seed.X < right_line[left_seed.Y])
					{
						right_line[left_seed.Y] = left_seed.X;
					}
					if (left_seed.X == right_border[left_seed.Y])//�ж��Ƿ���
					{
						r_lostline_num++;
					}
				}
			}
			if (left_seed.Y < USE_IMAGE_H - 2 && left_seed.Y>2)
			{
				if (use_image[left_seed.Y + 1][left_seed.X] == IMAGE_WHITE)
				{
					left_line_y[left_seed.X] = left_seed.Y;
				}
				if (use_image[left_seed.Y - 1][left_seed.X] == IMAGE_WHITE)
				{
					right_line_y[left_seed.X] = left_seed.Y;
				}
			}
		}
		else
		{
			break;//��������ʧ��������ѭ��,���⿨��
		}
	} while (left_seed.Y!=right_seed.Y || left_seed.X != right_seed.X);//�����Ӵ����½���������������������ѭ��
}

/***********************************************
* @brief :�õ�ͼ���������ֿ��ĺڿ�����������ڳ����ж�Ԫ�� 
* @param : ��ֵ��ͼ�����ұ���
* @return: �ڿ���
* @date  : 2022.9.16
* @author: ������
************************************************/
void CaculateBlackBlock()
{
	//�ж�ɨ�ߵķⶥ��
	if (leftline_endrow<=1)
	{
		for (uint8 column = left_line[1]; column < right_line[1]; column++)
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
	uint8 l_flag=3, r_flag=3;//������Ϊ�������ж�һ��״̬����Ϊһ��״̬�ǲ����������ֵģ�,��Ϊһ��ʼ��֪���Ƕ����ǲ������ȸ�һ��״̬֮���ֵ
	for (uint8 row = USE_IMAGE_H-5; row > leftline_endrow; row--)
	{
		//�Ӳ����ߵ�����
		if (left_line[row]==left_border[row] && left_line[row-2]==left_border[row-2] && left_line[row+2]!=left_border[row+2] && l_flag!=0)
		{
			black_block_num++;
			l_flag = 0;
		}
		if (right_line[row]==right_border[row] && right_line[row-2]==right_border[row-2] && right_line[row+2]!=right_border[row+2] && r_flag!=0)
		{
			black_block_num++;
			r_flag = 0;
		}
		//�Ӷ��ߵ�������
		if (left_line[row]!=left_border[row] && left_line[row-2]!=left_border[row-2] && left_line[row+2]==left_border[row+2] && l_flag != 1)
		{
			black_block_num++;
			l_flag = 1;
		}
		if (right_line[row]!=right_border[row] && right_line[row-2]!=right_border[row-2] && right_line[row+2]==right_border[row+2] && r_flag != 1)
		{
 			black_block_num++;
			r_flag = 1;
		}
		//˳������������»������
		center_line[row] = (left_line[row] + right_line[row]) / 2;
	}
}
