#include "ImageBasic.h"
#include "ImageWR.h"
#include "main.h"

#define GRAY_BLOCK   7//灰度扫线的模板大小BLOCK*BLOCK
#define CLIP_VALUE   2//灰度扫线的参数（1~5）
//=========================赛道特征变量=============================
//扫线得到的左右边线
myPoint left_line[EDGELINE_LENGTH],right_line[EDGELINE_LENGTH];//左右边线
uint8 l_line_count=0,r_line_count=0;//左右边线记录总共有多长
char l_lost_line[EDGELINE_LENGTH],r_lost_line[EDGELINE_LENGTH];//左右线是否丢线的记录数组
uint8 l_lostline_num = 0, r_lostline_num = 0;//左右丢线数
//================================================================

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
    int dif_gray_value;//灰度值差比和的值
    for (left_seed->Y=110-half-1,left_seed->X=USE_IMAGE_W/2; left_seed->X>half; left_seed->X--)
    {
        //灰度差比和=(f(x,y)-f(x-1,y))/(f(x,y)+f(x-1,y))
        dif_gray_value=100*(use_image[left_seed->Y][left_seed->X]-use_image[left_seed->Y][left_seed->X-1])
                       /(use_image[left_seed->Y][left_seed->X]+use_image[left_seed->Y][left_seed->X-1]);
        if(dif_gray_value>dif_thres) break;
    }
    for (right_seed->Y=110-half-1 ,right_seed->X=USE_IMAGE_W/2; right_seed->X<USE_IMAGE_W-half-1; right_seed->X++)
    {
        //灰度差比和=(f(x,y)-f(x-1,y))/(f(x,y)+f(x-1,y))
        dif_gray_value=100*(use_image[right_seed->Y][right_seed->X]-use_image[right_seed->Y][right_seed->X+1])
                       /(use_image[right_seed->Y][right_seed->X]+use_image[right_seed->Y][right_seed->X+1]);
        if(dif_gray_value>dif_thres) break;
    }
}

/***********************************************
* @brief : 八零域种子生长的规则，生长一次
* @param : uint8 half: 模板block/2
*          uint8 clip_value: 调整阈值（一般在1~5）
*          myPoint* seed:要进行生长的种子
*		   char choose: 选择左边还是右边的生长标号表歌
*		   uint8 *seed_num: 八零域的标号
* @return: 0：生长失败 1：生长成功
* @date  : 2023.4.12
* @author: 刘骏帆
************************************************/
char const eight_area_left[8][2]={{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1},{0,1},{1,1}};
char const eight_area_right[8][2]={{-1,0},{-1,-1},{0,-1},{1,-1},{1,0},{1,1},{0,1},{-1,1}};
uint8 EightAreasSeedGrown(myPoint* seed,char choose,uint8 *seed_num)
{
    uint8 half=GRAY_BLOCK/2;
    int point_num=0;
    //计算当前局部方块的阈值
    int local_thres = 0;
    uint8 gray=0;
    for (int dy = -half; dy <= half; dy++)
    {
        for (int dx = -half; dx <= half; dx++)
        {
            gray=use_image[seed->Y+dy][seed->X+dx];
            if(gray==IMAGE_BAN) continue;
            else
            {
                local_thres += gray;
                point_num++;
            }
        }
    }
    local_thres /= point_num;
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
        //Sobel算子
//        int tempx=(-  use_image[seed->Y+dy-1][seed->X+dx-1])
//              +(-2*use_image[seed->Y+dy  ][seed->X+dx-1])
//              +(-  use_image[seed->Y+dy+1][seed->X+dx-1])
//              +(   use_image[seed->Y+dy-1][seed->X+dx+1])
//              +( 2*use_image[seed->Y+dy  ][seed->X+dx+1])
//              +(   use_image[seed->Y+dy+1][seed->X+dx+1]);
//        if(tempx<0)
//            tempx=-tempx;
//
//        int tempy=(   use_image[seed->Y+dy+1][seed->X+dx-1])
//              +( 2*use_image[seed->Y+dy+1][seed->X+dx  ])
//              +(   use_image[seed->Y+dy+1][seed->X+dx+1])
//              +(-  use_image[seed->Y+dy-1][seed->X+dx-1])
//              +(-2*use_image[seed->Y+dy-1][seed->X+dx  ])
//              +(-  use_image[seed->Y+dy-1][seed->X+dx+1]);
//        if(tempy<0)
//            tempy=-tempy;
//        int temp=tempx+tempy;
        uint8 next_value=use_image[seed->Y+dy][seed->X+dx];
        if (next_value<local_thres)
        {
            seed->X += dx;
            seed->Y += dy;
            if (*seed_num-2<0) *seed_num+=6;
            else               *seed_num-=2;
            return 1;
        }
        else if (seed->Y+dy<=half || seed->Y+dy>=USE_IMAGE_H-half-1 || seed->X+dx<=half || seed->X+dx>=USE_IMAGE_W-half-1)//这边的判断是判断种子生长到了图像边缘，但是由于上一次判断了不是黑点，所以就不用再次判断该点是不是白点了
        {
            seed->X += dx;
            seed->Y += dy;
            if (*seed_num-2<0) *seed_num+=6;
            else               *seed_num-=2;
            return 2;
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
* @date  : 2023.4.12
* @author: 刘骏帆
* @note	 :左种子 3 2 1  右种子 1 2 3
*				4 S 0		 0 S 4
*				5 6 7	     7 6 5
************************************************/
uint8 left_seed_num=0,right_seed_num=0;//八零域的种子生长标号，设置成全局变量，为了给再次生长的时候找到标号
void EdgeDetection(void)
{
    /*播种*/
    myPoint left_seed,right_seed;
    SowSeedGray(GRAY_BLOCK/2, GRAY_DIF_THRES, &left_seed, &right_seed);
    /*种子生长*/
    left_seed_num=0,right_seed_num=0;
    uint8 grow_success_flag=0;//种子生长成功的标志变量，即：种子开始记录到边线之后flag=1
    //左边线生长
    while(l_line_count<EDGELINE_LENGTH)
    {
        uint8 seed_grow_result=EightAreasSeedGrown(&left_seed,'l',&left_seed_num);
        if (seed_grow_result==1)
        {
            grow_success_flag=1;
            left_line[l_line_count].X=left_seed.X;left_line[l_line_count].Y=left_seed.Y;
            l_line_count++;
        }
        else if(seed_grow_result==2)
        {
            if(grow_success_flag==0 && left_seed.Y>90)//最下面很快就丢线那么就继续扫线，直到扫到有的时候才记录
                continue;
            else
                break;
        }
        else break;
    }
    grow_success_flag=0;
    //右边线生长
    while(r_line_count<EDGELINE_LENGTH)
    {
        uint8 seed_grow_result=EightAreasSeedGrown(&right_seed,'r',&right_seed_num);
        if (seed_grow_result==1)
        {
            grow_success_flag=1;
            right_line[r_line_count].X=right_seed.X;right_line[r_line_count].Y=right_seed.Y;
            r_line_count++;
        }
        else if(seed_grow_result==2)
        {
            if(grow_success_flag==0 && right_seed.Y>90)//最下面很快就丢线那么就继续扫线，直到扫到有的时候才记录
                continue;
            else
                break;
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
        //计算当前局部方块的阈值
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
        //右手迷宫巡线
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
