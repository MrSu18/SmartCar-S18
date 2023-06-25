/*
 * debug.c
 *
 *  Created on: 2023��4��14��
 *      Author: 30516
 */
#include "ImageConversion.h"
#include "ImageTrack.h"
#include "zf_device_tft180.h"
#include "KeyMenu.h"
#include "debug.h"

#define PER_IMAGE_W     188             //��͸��ͼ��Ŀ��
#define PER_IMAGE_H     120             //��͸��ͼ��ĸ߶�
#define PER_IMG     gray_image    //����͸�ӱ任��ͼ��
#define IMAGE_BAN   127             //��͸�ӽ�ֹ����ĻҶ�ֵ

inline int Limit(int x, int low, int up)//��x���������޷�
{
    return x > up ? up : x < low ? low : x;
}

/***********************************************
* @brief : LCD��ʾ͸�Ӻ��ͼ��
* @param : ��
* @return: ��
* @date  : 2023.4.14
* @author: ������
************************************************/
void LCDShowPerImage(void)
{
    uint8 per_image[USE_IMAGE_H][USE_IMAGE_W];
    //��͸�Ӿ���
    double change_un_Mat[3][3] ={{-2.289773,2.573377,-129.929495},{-0.000000,0.964493,-164.411306},{-0.000000,0.027523,-3.691638}};
    for (int i = 0; i < PER_IMAGE_W; i++)
    {
        for (int j = 0; j < PER_IMAGE_H; j++)
        {
            int local_x = (int)((change_un_Mat[0][0]*i+change_un_Mat[0][1]*j+change_un_Mat[0][2])
                               /(change_un_Mat[2][0]*i+change_un_Mat[2][1]*j+change_un_Mat[2][2]));
            int local_y = (int)((change_un_Mat[1][0]*i+change_un_Mat[1][1]*j+change_un_Mat[1][2])
                               /(change_un_Mat[2][0]*i+change_un_Mat[2][1]*j+change_un_Mat[2][2]));
            if (local_x>= 0 && local_y >= 0 && local_y < MT9V03X_H && local_x < MT9V03X_W)
            {
                per_image[j][i] = PER_IMG[local_y][local_x];
            }
            else
            {
                per_image[j][i] = IMAGE_BAN;          //&PER_IMG[0][0];
            }
        }
    }
    tft180_show_gray_image(0, 0, per_image[0], MT9V03X_W, MT9V03X_H, 160, 120, 0);
}

/***********************************************
* @brief : LCD��ʾһ����������(��������)
* @param : myPoint_f* line: Ҫ��ʾ�ı���
*          uint8 len: �������鳤��
*          const uint16 color: ��ɫ
* @return: ��
* @date  : 2023.4.14
* @author: ������
************************************************/
void LCDShowFloatLine(myPoint_f* line,int len,const uint16 color)
{
    for(int i=0;i<len;i++)
    {
        float y=line[i].Y,x=line[i].X*0.8510638297872340425531914893617;
        if(x<0 || x>159 || y<0 || y>120) continue;
        tft180_draw_point((uint16)x, (uint16)y, color);
    }
}

/***********************************************
* @brief : LCD��ʾһ����������(����)
* @param : myPoint* line: Ҫ��ʾ�ı���
*          uint8 len: �������鳤��
*          const uint16 color: ��ɫ
* @return: ��
* @date  : 2023.4.14
* @author: ������
************************************************/
void LCDShowUint8Line(myPoint* line,int len,const uint16 color)
{
    for(int i=0;i<len;i++)
    {
        uint8 y=line[i].Y,x=line[i].X*0.8510638297872340425531914893617;
        if(x<0 || x>159 || y<0 || y>120) continue;
        tft180_draw_point((uint16)x, (uint16)y, color);
    }
}

/***********************************************
* @brief : LCD��ʾһ������
* @param : uint8 row:��
*          const uint16 color: ��ɫ
* @return: ��
* @date  : 2023.4.19
* @author: ������
************************************************/
void LCDDrowRow(uint8 row,const uint16 color)
{
   for(uint8 i=0;i<MT9V03X_W-1;i++)
   {
       tft180_draw_point(i, row, color);
   }
}

/***********************************************
* @brief : LCD��ʾһ������
* @param : uint8 column:��
*          const uint16 color: ��ɫ
* @return: ��
* @date  : 2023.4.19
* @author: ������
************************************************/
void LCDDrowColumn(uint8 column,const uint16 color)
{
    uint16 temp=(uint16)(column*0.8510638297872340425531914893617);
    for(uint8 i=0;i<MT9V03X_H;i++)
    {
        tft180_draw_point(temp, i, color);
    }
}

/***********************************************
* @brief : LCDʮ�ֱ��һ����
* @param : uint8 column:��
*          const uint16 color: ��ɫ
* @return: ��
* @date  : 2023.4.19
* @author: ������
************************************************/
void LCDDrowPoint(uint8 row,uint8 column,const uint16 color)
{
    LCDDrowRow(row,color);
//    LCDDrowColumn(column,color);
}

/***********************************************
* @brief : �ж��Ƿ���ʾͼ��
* @param : void
* @return: void
* @date  : 2023.5.2
* @author: ������
************************************************/
void ShowImage(void)
{
    if(gray_image_flag == 1)
    {
        tft180_show_gray_image(0, 0, gray_image[0], MT9V03X_W, MT9V03X_H, 160, 120, 0);
        tft180_show_float(98, 0, image_bias, 2, 3);
    }
    else if(per_image_flag == 1)
    {
        LCDShowPerImage();
        tft180_show_float(98, 0, image_bias, 2, 3);
    }
}

/***********************************************
* @brief : �ж��Ƿ���ʾ���߻�����
* @param : void
* @return: void
* @date  : 2023.5.2
* @author: ������
************************************************/
void ShowLine(void)
{
    if(per_edgeline_flag == 1)
    {
        LCDShowFloatLine(f_left_line1,per_l_line_count,RGB565_BLUE);
        LCDShowFloatLine(f_right_line1,per_r_line_count,RGB565_RED);
        tft180_show_int(98, 30, per_l_line_count, 3);
        tft180_show_int(98, 60, per_r_line_count, 3);
    }
    //��ʾ����
    if(c_line_flag == 1)
    {
        int num=Limit(round(aim_distance / SAMPLE_DIST), 0, c_line_count);
        for(int i=0;i<num;i++)
        {
            float y=center_line[i].Y,x=center_line[i].X*0.8510638297872340425531914893617;
            if(x<0 || x>159 || y<0 || y>120) continue;
            tft180_draw_point((uint16)x, (uint16)y, RGB565_GREEN);
        }
    }
    if(edgeline_flag == 1)
    {
        LCDShowUint8Line(left_line,l_line_count,RGB565_BLUE);
        LCDShowUint8Line(right_line,r_line_count,RGB565_RED);
        tft180_show_int(98, 30, l_line_count, 3);
        tft180_show_int(98, 60, r_line_count, 3);
    }
}
/***********************************************
* @brief : �����õ�LED�Ƴ�ʼ��
* @param : void
* @return: void
* @date  : 2023.5.31
* @author: L
************************************************/
void LedInit(void)
{
    //���İ�����ͨ��led
    gpio_init(P20_8, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    gpio_init(P20_9, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    gpio_init(P21_5, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    gpio_init(P21_4, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    //RGB��
//    gpio_init(P23_1, GPO, GPIO_LOW, GPO_PUSH_PULL);
    pwm_init(GREEN, 12500, 10000);
    pwm_init(RED,12500,10000);
    pwm_init(BLUE,12500,10000);
}
/***********************************************
* @brief : ��RGB�Ƶ���ɫ,pwmԽС��ɫԽ��
* @param : void
* @return: green_pwm:�̵Ƶ�pwm
*          red_pwm:��Ƶ�pwm
*          blue_pwm:���Ƶ�pwm
* @date  : 2023.5.31
* @author: L
************************************************/
void ColorOfRGB(int green_pwm,int red_pwm,int blue_pwm)
{
    pwm_set_duty(GREEN, green_pwm);
    pwm_set_duty(RED, red_pwm);
    pwm_set_duty(BLUE, blue_pwm);
}
