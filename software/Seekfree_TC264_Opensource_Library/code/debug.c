/*
 * debug.c
 *
 *  Created on: 2023年4月14日
 *      Author: 30516
 */
#include "ImageConversion.h"
#include "ImageTrack.h"
#include "zf_device_tft180.h"
#include "KeyMenu.h"
#include "debug.h"

#define PER_IMAGE_W     188             //逆透视图像的宽度
#define PER_IMAGE_H     120             //逆透视图像的高度
#define PER_IMG     gray_image    //用于透视变换的图像
#define IMAGE_BAN   127             //逆透视禁止区域的灰度值

inline int Limit(int x, int low, int up)//给x设置上下限幅
{
    return x > up ? up : x < low ? low : x;
}

/***********************************************
* @brief : LCD显示透视后的图像
* @param : 无
* @return: 无
* @date  : 2023.4.14
* @author: 刘骏帆
************************************************/
void LCDShowPerImage(void)
{
    uint8 per_image[USE_IMAGE_H][USE_IMAGE_W];
    //逆透视矩阵
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
* @brief : LCD显示一条边线数组(浮点类型)
* @param : myPoint_f* line: 要显示的边线
*          uint8 len: 边线数组长度
*          const uint16 color: 颜色
* @return: 无
* @date  : 2023.4.14
* @author: 刘骏帆
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
* @brief : LCD显示一条边线数组(整型)
* @param : myPoint* line: 要显示的边线
*          uint8 len: 边线数组长度
*          const uint16 color: 颜色
* @return: 无
* @date  : 2023.4.14
* @author: 刘骏帆
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
* @brief : LCD显示一条横线
* @param : uint8 row:行
*          const uint16 color: 颜色
* @return: 无
* @date  : 2023.4.19
* @author: 刘骏帆
************************************************/
void LCDDrowRow(uint8 row,const uint16 color)
{
   for(uint8 i=0;i<MT9V03X_W-1;i++)
   {
       tft180_draw_point(i, row, color);
   }
}

/***********************************************
* @brief : LCD显示一条横线
* @param : uint8 column:行
*          const uint16 color: 颜色
* @return: 无
* @date  : 2023.4.19
* @author: 刘骏帆
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
* @brief : LCD十字标记一个点
* @param : uint8 column:行
*          const uint16 color: 颜色
* @return: 无
* @date  : 2023.4.19
* @author: 刘骏帆
************************************************/
void LCDDrowPoint(uint8 row,uint8 column,const uint16 color)
{
    LCDDrowRow(row,color);
//    LCDDrowColumn(column,color);
}

/***********************************************
* @brief : 判断是否显示图像
* @param : void
* @return: void
* @date  : 2023.5.2
* @author: 刘骏帆
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
* @brief : 判断是否显示边线或中线
* @param : void
* @return: void
* @date  : 2023.5.2
* @author: 刘骏帆
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
    //显示中线
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
* @brief : 调试用的LED灯初始化
* @param : void
* @return: void
* @date  : 2023.5.31
* @author: L
************************************************/
void LedInit(void)
{
    //核心板上普通的led
    gpio_init(P20_8, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    gpio_init(P20_9, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    gpio_init(P21_5, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    gpio_init(P21_4, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    //RGB灯
//    gpio_init(P23_1, GPO, GPIO_LOW, GPO_PUSH_PULL);
    pwm_init(GREEN, 12500, 10000);
    pwm_init(RED,12500,10000);
    pwm_init(BLUE,12500,10000);
}
/***********************************************
* @brief : 调RGB灯的颜色,pwm越小颜色越亮
* @param : void
* @return: green_pwm:绿灯的pwm
*          red_pwm:红灯的pwm
*          blue_pwm:蓝灯的pwm
* @date  : 2023.5.31
* @author: L
************************************************/
void ColorOfRGB(int green_pwm,int red_pwm,int blue_pwm)
{
    pwm_set_duty(GREEN, green_pwm);
    pwm_set_duty(RED, red_pwm);
    pwm_set_duty(BLUE, blue_pwm);
}
