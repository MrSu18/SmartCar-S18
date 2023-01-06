#include "ImageBasic.h"
//============================����================================
#define TRACK_HALF_WIDTH	13//����������ص�
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
*		   char choose: ѡ����߻����ұߵ�������ű��
 *		   uint8 *seed_count: ��������Ѿ��ڰ˸����������˶��ٴ��ˣ���������˻�û����˵������ʧ����
 *		   uint8 *seed_num: ������ı��
* @return: 0������ʧ�� 1�������ɹ�
* @date  : 2023.1.7
* @author: ������
************************************************/
char const eight_area_left[8][2]={{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1},{0,1},{1,1}};
char const eight_area_right[8][2]={{-1,0},{-1,-1},{0,-1},{1,-1},{1,0},{1,1},{0,1},{-1,1}};
uint8 EightAreasSeedGrown(myPoint* seed,char choose,uint8 *seed_count,uint8 *seed_num)
{
    uint8 next_value=0;
    char dx=0,dy=0;
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
        *seed_count=0;
        if (*seed_num-2<0) *seed_num+=6;
        else               *seed_num-=2;
    }
    else
    {
        *seed_num = (*seed_num + 1) % 8;
        *seed_count++;
    }
    if(*seed_count==8)
        return 0;
    else
        return 1;
}

/***********************************************
* @brief : ������ɨ�ߺ���
* @param : ������ֵ��ͼ��
* @return: ���ұ��ߡ����Ҷ�����
* @date  : 2023.1.7
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
    uint8 left_seed_num=0,left_seed_count=0,right_seed_num=0,right_seed_count=0;
    uint8 change_lr_flag=0,left_finish=0,right_finish=0;//change_lr_flag=0:������� change_lr_flag=1:�ұ�����
    do
	{
        if(change_lr_flag == 0 && left_finish==0)
        {
            if (EightAreasSeedGrown(&left_seed,'l',&left_seed_count,&left_seed_num) == 1)//����һ��
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
            if (EightAreasSeedGrown(&right_seed,'r',&right_seed_count,&right_seed_num) == 1)
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
