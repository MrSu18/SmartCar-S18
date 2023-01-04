#include "ImageBasic.h"
//============================����================================
#define TRACK_HALF_WIDTH	13//����������ص�
uint8 const seed_l_flag[8] = { 0,1,2,3,4,5,6,7 };//�����ӱ�Ŷ���������
uint8 const seed_r_flag[8] = { 4,3,2,1,0,7,6,5 };//�����ӱ�Ŷ���������
//===============================================================

//=========================������������=============================
myPoint left_line[EDGELINE_LENGTH], center_line[EDGELINE_LENGTH], right_line[EDGELINE_LENGTH];//����������
uint8 l_line_count=0,c_line_count=0,r_line_count=0;//�����ұ��߼�¼�ܹ��ж೤
uint8 l_lostline_num = 0, r_lostline_num = 0;//���Ҷ�����
uint8 black_block_num = 0;//�����ڿ����
uint8 track_top_row = USE_IMAGE_H;//��������У�Ҳ��������е�Ч��
//================================================================
uint8 left_line_x[USE_IMAGE_H], center_line_x[USE_IMAGE_H], right_line_x[USE_IMAGE_H];//����������
/***********************************************
* @brief : ɨ�ߵĲ��ֺ������õ�����һ�������ߵ�����
* @param : ��ֵ��ͼ��
* @return: �����ӡ�������
* @date  : 2022.9.7
* @author: ������
************************************************/
#define BLACK_CONTINUE_WIDTH_THR	7//���м������ߵĺ�ɫ��������ֵ
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
                left_line[l_line_count].X=left_seed->X;left_line[l_line_count].Y=left_seed->Y;
                l_line_count++;
				break;
			}
			else continue;
		}
	}
	//�ҵ��ұߵ�����
	for (uint8 column = left_seed->X; column < right_border[right_seed->Y]-1; column++)
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
                right_line[r_line_count].X=right_seed->X;right_line[r_line_count].Y=right_seed->Y;
                r_line_count++;
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
			if (seed->X+1<=USE_IMAGE_W-1)//��ֹ��������Խ��
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
    /*1.�����ҵ���������*/
	myPoint left_seed, right_seed;//�����ߵĳ�ʼ����
	left_seed.X = left_border[USE_IMAGE_H - 2]; left_seed.Y = USE_IMAGE_H - 2; right_seed.X = right_border[USE_IMAGE_H - 2]; right_seed.Y = USE_IMAGE_H - 2;//��ʼ��Ϊ0
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
    SeedGrowAqueue l_seed_queue,r_seed_queue;//���������ڱ��ת�Ƶ�ѭ������
    l_seed_queue.front = 0; l_seed_queue.rear = 7; r_seed_queue.front = 0; r_seed_queue.rear = 7;
    uint8 change_lr_flag=0,left_finish=0,right_finish=0;//change_lr_flag=0:������� change_lr_flag=1:�ұ�����
	do
	{
        if(change_lr_flag == 0 && left_finish==0)
        {
            if (EightAreasSeedGrown(&left_seed, &l_seed_queue, seed_l_flag) == 1)//����һ��
            {
                LCDDrawPoint(left_seed.Y, left_seed.X);
                if(left_seed.Y==0 || left_seed.X==right_border[left_seed.Y])//��������������ͼ���ϱ߽���ұ߽�˵��ɨ�������
                {
                    change_lr_flag=!change_lr_flag;
                    left_finish=1;
                }
                else if(left_seed.X==left_border[left_seed.Y] )//����������������߽�
                {
                    if(right_finish!=1)  change_lr_flag=!change_lr_flag;
                }
            }
            else
            {
                break;//��������ʧ��������ѭ��,���⿨��
            }
        }
        else if(change_lr_flag==1 && right_finish==0)
        {
            if (EightAreasSeedGrown(&right_seed, &r_seed_queue, seed_r_flag) == 1)
            {
                LCDDrawPoint(right_seed.Y, right_seed.X);
                if(right_seed.Y==0 || right_seed.X==left_border[right_seed.Y])//��������������ͼ���ϱ߽磬��߽�
                {
                    change_lr_flag=!change_lr_flag;
                    right_finish=1;
                }
                else if(right_seed.X==right_border[right_seed.Y])//���������������ұ߽�
                {
                    if(left_finish!=1)  change_lr_flag=!change_lr_flag;
                }
            }
            else
            {
                break;//��������ʧ��������ѭ��,���⿨��
            }
        }
        else break;
	} while (left_seed.Y!=right_seed.Y || left_seed.X != right_seed.X);//�������Ӻ������Ӻϲ���ɨ�߽���
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
	uint8 l_flag=3, r_flag=3;//������Ϊ�������ж�һ��״̬����Ϊһ��״̬�ǲ����������ֵģ�,��Ϊһ��ʼ��֪���Ƕ����ǲ������ȸ�һ��״̬֮���ֵ
	for (uint8 row = USE_IMAGE_H-5; row > track_top_row; row--)
	{
		//�Ӳ����ߵ�����
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
		//�Ӷ��ߵ�������
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
		//˳������������»������
		center_line_x[row] = (left_line_x[row] + right_line_x[row]) / 2;
	}
}
