#include <string.h>
#include "ImageBasic.h"
#include "zf_common_headfile.h"
//=========================������������=============================
myPoint left_line[EDGELINE_LENGTH]={0},right_line[EDGELINE_LENGTH]={0};//���ұ���
char l_lost_line[EDGELINE_LENGTH]={0},r_lost_line[EDGELINE_LENGTH]={0};//�������Ƿ��ߵļ�¼����
uint8 l_line_count=0,r_line_count=0;//���ұ��߼�¼�ܹ��ж೤
uint8 l_lostline_num = 0, r_lostline_num = 0;//���Ҷ�����
//================================================================

/***********************************************
* @brief : ɨ�ߵĲ��ֺ������õ�����һ�������ߵ�����
* @param : ��ֵ��ͼ��
* @return: �����ӡ�������
* @date  : 2022.9.7
* @author: ������
************************************************/
#define BLACK_CONTINUE_WIDTH_THR	5//���м������ߵĺ�ɫ��������ֵ
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
				break;
			}
			else continue;
		}
	}
}

/***********************************************
* @brief : ͨ����Ⱥ��㷨���ҵ���������
* @param : �Ҷ�ͼ��
* @return: �����ӡ�������
* @date  : 2023.4.12
* @author: ������
************************************************/
#define GRAY_DIF_THRES  10//�ҶȲ�Ⱥ��㷨����ֵ
void SowSeedGray(uint8 half, char dif_thres, myPoint *left_seed, myPoint *right_seed)//ͨ����Ⱥ��㷨���ҵ���������
{
    for (left_seed->Y = USE_IMAGE_H_MAX - half - 1, left_seed->X = USE_IMAGE_W / 2;left_seed->X > half; left_seed->X--)
    {
        if (PointSobelTest(*left_seed) == 1) break;
    }
    if (left_seed->X == half)//û�гɹ�����
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
    if (right_seed->X == USE_IMAGE_W - half - 1)//û�гɹ�����
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
* @brief : sobel�ݶȼ��
* @param : Ҫ�����sobel�ĵ�
* @return: 1���õ����������� 0���õ㲻��������Ե
* @date  : 2023.4.21
* @author: ������
************************************************/
#define SOBEL_THRES 20//sobel�ݶ���ֵ���ھ��Ǳ���
uint8 PointSobelTest(myPoint a)//���ص��sobel����
{
    if (a.X<2 || a.X>USE_IMAGE_W-2-1 || a.Y<2 || a.Y>USE_IMAGE_H-2-1) return 0;//Խ��
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
* @brief : ���������������Ĺ�������һ��(��ֵ��ͼ)
* @param : myPoint* seed:Ҫ��������������
*		   char choose: ѡ����߻����ұߵ�������ű��
*		   uint8 *seed_num: ������ı��
* @return: 0������ʧ�� 1�������ɹ� 2���������˶�������
* @date  : 2023.3.5
* @author: ������
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
        else if (seed->Y+dy<=0 || seed->Y+dy>=USE_IMAGE_H-1 || seed->X+dx<=left_border[seed->Y+dy] || seed->X+dx>=right_border[seed->Y+dy])//��ߵ��ж����ж�������������ͼ���Ե������������һ���ж��˲��Ǻڵ㣬���ԾͲ����ٴ��жϸõ��ǲ��ǰ׵���
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
    return 0;//ѭ��������û�ҵ�˵���������Ҳ�����break
}

/***********************************************
* @brief : ���������������Ĺ�������һ��(�Ҷ�ͼ)
* @param : uint8 half: ģ��block/2
*          uint8 clip_value: ������ֵ��һ����1~5��
*          myPoint* seed:Ҫ��������������
*          char choose: ѡ����߻����ұߵ�������ű��
*          uint8 *seed_num: ������ı��
* @return: 0������ʧ�� 1�������ɹ�
* @date  : 2023.4.12
* @author: ������
************************************************/
uint8 EightAreasSeedGrownGray(myPoint* seed,char choose,uint8 *seed_num)
{
    uint8 half=GRAY_BLOCK/2;
    //���㵱ǰ�ֲ��������ֵ
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
        if (seed->Y+dy<=USE_IMAGE_H_MIN+half || seed->Y+dy>=USE_IMAGE_H-half-1 || seed->X+dx<=half || seed->X+dx>=USE_IMAGE_W-half-1)//��ߵ��ж����ж�������������ͼ���Ե������������һ���ж��˲��Ǻڵ㣬���ԾͲ����ٴ��жϸõ��ǲ��ǰ׵���
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
    return 0;//ѭ��������û�ҵ�˵���������Ҳ�����break
}

/***********************************************
* @brief : ������ɨ�ߺ���
* @param : �����Ҷ�ͼ
* @return: ���ұ��ߡ����Ҷ�����
* @date  : 2023.4.21
* @author: ������
* @note  :������ 3 2 1  ������ 1 2 3
*               4 S 0        0 S 4
*               5 6 7        7 6 5
************************************************/
uint8 left_seed_num=0,right_seed_num=0;//�����������������ţ����ó�ȫ�ֱ�����Ϊ�˸��ٴ�������ʱ���ҵ����
void EdgeDetection(void)
{
    /*����*/
    myPoint left_seed,right_seed;
    SowSeedGray(GRAY_BLOCK/2, GRAY_DIF_THRES, &left_seed, &right_seed);
    left_line[l_line_count]=left_seed;l_line_count++;
    right_line[r_line_count]=right_seed;r_line_count++;
    /*��������*/
    left_seed_num=0,right_seed_num=0;
    //���������
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
    //�ұ�������
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

