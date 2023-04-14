/*
 * debug.c
 *
 *  Created on: 2023年4月14日
 *      Author: 30516
 */
#include "ImageConversion.h"
#include "ImageTrack.h"
#include "zf_device_tft180.h"

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
    double change_un_Mat[3][3] ={{-2.823035,3.058385,-152.620928},{0.141489,1.441372,-256.109755},{0.002246,0.033146,-4.763373}};
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
