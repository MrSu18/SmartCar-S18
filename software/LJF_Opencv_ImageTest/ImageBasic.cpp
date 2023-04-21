#include "ImageBasic.h"
#include "ImageWR.h"
#include "main.h"

//=========================������������=============================
//ɨ�ߵõ������ұ���
myPoint left_line[EDGELINE_LENGTH],right_line[EDGELINE_LENGTH];//���ұ���
uint8 l_line_count=0,r_line_count=0;//���ұ��߼�¼�ܹ��ж೤
char l_lost_line[EDGELINE_LENGTH],r_lost_line[EDGELINE_LENGTH];//�������Ƿ��ߵļ�¼����
uint8 l_lostline_num = 0, r_lostline_num = 0;//���Ҷ�����
//================================================================

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
    int dif_gray_value;//�Ҷ�ֵ��Ⱥ͵�ֵ
    for (left_seed->Y = USE_IMAGE_H_MAX - half - 1, left_seed->X = USE_IMAGE_W / 2;left_seed->X > half; left_seed->X--)
    {
        if (PointSobelTest(*left_seed) == 1) break;
//        //�ҶȲ�Ⱥ�=(f(x,y)-f(x-1,y))/(f(x,y)+f(x-1,y))
//        dif_gray_value=100*(use_image[left_seed->Y][left_seed->X]-use_image[left_seed->Y][left_seed->X-1])
//                       /(use_image[left_seed->Y][left_seed->X]+use_image[left_seed->Y][left_seed->X-1]);
//        if(dif_gray_value>dif_thres) break;
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
        //�ҶȲ�Ⱥ�=(f(x,y)-f(x-1,y))/(f(x,y)+f(x-1,y))
//        dif_gray_value=100*(use_image[right_seed->Y][right_seed->X]-use_image[right_seed->Y][right_seed->X+1])
//                       /(use_image[right_seed->Y][right_seed->X]+use_image[right_seed->Y][right_seed->X+1]);
//        if(dif_gray_value>dif_thres) break;
    }
    if (right_seed->X == USE_IMAGE_W - half - 1)//û�гɹ�����
    {
        for (; right_seed->Y > 90; right_seed->Y--)
        {
            if (PointSobelTest(*right_seed) == 1)
            {
                right_seed->X++;
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
#define SOBEL_THRES 30//sobel�ݶ���ֵ���ھ��Ǳ���
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
    if(sobel_result>SOBEL_THRES) return 1;
    else                         return 0;
}

/***********************************************
* @brief : ���������������Ĺ�������һ��
* @param : uint8 half: ģ��block/2
*          uint8 clip_value: ������ֵ��һ����1~5��
*          myPoint* seed:Ҫ��������������
*		   char choose: ѡ����߻����ұߵ�������ű��
*		   uint8 *seed_num: ������ı��
* @return: 0������ʧ�� 1�������ɹ�
* @date  : 2023.4.12
* @author: ������
************************************************/
char const eight_area_left[8][2]={{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1},{0,1},{1,1}};
char const eight_area_right[8][2]={{-1,0},{-1,-1},{0,-1},{1,-1},{1,0},{1,1},{0,1},{-1,1}};
uint8 EightAreasSeedGrownGray(myPoint* seed, char choose, uint8 *seed_num)
{
    uint8 half=GRAY_BLOCK/2;
    //���㵱ǰ�ֲ��������ֵ
    int local_thres = 0;
    static uint8 gray=127;
    for (int dy = -half; dy <= half; dy++)
    {
        for (int dx = -half; dx <= half; dx++)
        {
            local_thres += use_image[seed->Y+dy][seed->X+dx];
        }
    }
    local_thres+=gray;
    local_thres /= (GRAY_BLOCK*GRAY_BLOCK+1);
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
        if (seed->Y+dy<USE_IMAGE_H_MIN+half || seed->Y+dy>USE_IMAGE_H-half-1 || seed->X+dx<half || seed->X+dx>USE_IMAGE_W-half-1)//��ߵ��ж����ж�������������ͼ���Ե������������һ���ж��˲��Ǻڵ㣬���ԾͲ����ٴ��жϸõ��ǲ��ǰ׵���
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
* @date  : 2023.4.12
* @author: ������
* @note	 :������ 3 2 1  ������ 1 2 3
*				4 S 0		 0 S 4
*				5 6 7	     7 6 5
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
//        LCDDrawPoint(left_seed.Y,left_seed.X,0,255,0);
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

const int dir_front[4][2] = {{0,-1},{1,0},{0,1},{-1,0}};
const int dir_frontleft[4][2] = {{-1,-1},{1,-1},{1, 1},{-1,1}};
const int dir_frontright[4][2] = {{1,-1},{1,1},{-1, 1},{-1,-1}};
void Findline_Lefthand_Adaptive(int block_size, int clip_value,myPoint left_seed,myPoint* left_line, uint8 *num)
{
    int half = block_size / 2;
    int step = 0, dir = 0, turn = 0,point_num=0;
    while (step < *num && turn < 4)
    {
        //���㵱ǰ�ֲ��������ֵ
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
        //�����Թ�Ѳ��
        int current_value = use_image[left_seed.Y][left_seed.X];
        int front_value = use_image[left_seed.Y+dir_front[dir][1]][left_seed.X+dir_front[dir][0]];
        int frontleft_value = use_image[left_seed.Y+dir_frontleft[dir][1]][left_seed.X+dir_frontleft[dir][0]];
        if (front_value < local_thres ||half>left_seed.Y||left_seed.Y+dir_front[dir][1]>USE_IMAGE_H-half-1||half>left_seed.X+dir_front[dir][0]||left_seed.X+dir_front[dir][0]>USE_IMAGE_W-half-1)
        {
            dir = (dir + 1) % 4;
            turn++;
        }
        else if (frontleft_value < local_thres)
        {
            left_seed.X += dir_front[dir][0];
            left_seed.Y += dir_front[dir][1];
            left_line[step].X = left_seed.X;
            left_line[step].Y = left_seed.Y;
            step++;
            turn = 0;
        }
        else
        {
            left_seed.X += dir_frontleft[dir][0];
            left_seed.Y += dir_frontleft[dir][1];
            dir = (dir + 3) % 4;
            left_line[step].X = left_seed.X;
            left_line[step].Y = left_seed.Y;
            step++;
            turn = 0;
        }
        //printf("local_thres=%d,front_value=%d,frontleft_value=%d,dir=%d\n",local_thres,front_value,frontleft_value,dir);
//        UseImageDataToUseMat();
//        LCDDrawPoint(left_seed.Y,left_seed.X,0,255,0);
    }
    *num = step;
}

void Findline_Righthand_Adaptive(int block_size, int clip_value, myPoint right_seed, myPoint* right_line, uint8 *num)
{
    int half = block_size / 2;
    int step = 0, dir = 0, turn = 0,point_num=0;
    while (step < *num && left_border[right_seed.Y]+half < right_seed.X && right_seed.X < right_border[right_seed.Y] - half - 1 && half < right_seed.Y && right_seed.Y < USE_IMAGE_H - half - 1 && turn < 4)
    {
        //���㵱ǰ�ֲ��������ֵ
        int local_thres = 0;
        uint8 gray=0;
        for (int dy = -half; dy <= half; dy++)
        {
            for (int dx = -half; dx <= half; dx++)
            {
                gray=use_image[right_seed.Y+dy][right_seed.X+dx];
                if(gray==IMAGE_BAN) continue;
                else
                {
                    local_thres += gray;
                    point_num++;
                }
            }
        }
        local_thres /= point_num;
        local_thres -= clip_value;
        point_num=0;
        //�����Թ�Ѳ��
        int current_value = use_image[right_seed.Y][right_seed.X];
        int front_value = use_image[right_seed.Y+dir_front[dir][1]][right_seed.X+dir_front[dir][0]];
        int frontright_value = use_image[right_seed.Y+dir_frontright[dir][1]][right_seed.X+dir_frontright[dir][0]];
        if (front_value < local_thres)
        {
            dir = (dir + 3) % 4;
            turn++;
        }
        else if (frontright_value < local_thres)
        {
            right_seed.X += dir_front[dir][0];
            right_seed.Y += dir_front[dir][1];
            right_line[step].X = right_seed.X;
            right_line[step].Y = right_seed.Y;
            step++;
            turn = 0;
        }
        else
        {
            right_seed.X += dir_frontright[dir][0];
            right_seed.Y += dir_frontright[dir][1];
            dir = (dir + 1) % 4;
            right_line[step].X = right_seed.X;
            right_line[step].Y = right_seed.Y;
            step++;
            turn = 0;
        }
    }
    *num = step;
}
