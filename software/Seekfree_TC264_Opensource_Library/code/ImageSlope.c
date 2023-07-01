/*
 * ImageSlope.c
 *
 *  Created on: 2023年4月16日
 *      Author: L
 */
#include "zf_common_headfile.h"
#include "ImageTrack.h"
#include "motor.h"
#include "ImageSpecial.h"
#include "Control.h"
#include "stdlib.h"
#include "icm20602.h"
#include "adc.h"
#include "debug.h"

typedef enum SlopeType
{
    kSlopeBegin = 0,
    kSlopeUp,
    kSlopeEnd,
}SlopeType;//坡道状态机状态结构体

SlopeType slope_type = kSlopeBegin;

/***********************************************
* @brief : 坡道状态机
* @param : void
* @return: 0:不是坡道
*          1:识别到坡道
* @date  : 2023.4.21
* @author: L
************************************************/
uint8 SlopeIdentify(void)
{
    switch(slope_type)
    {
        case kSlopeBegin://求左右两个边线的斜率，斜率相反且斜率在0.2到0.5之间
        {
            dl1a_get_distance();//测距
            float gradient_l = 0;
            float gradient_r = 0;
            gradient_l = CalculateGradient('l');//求左线斜率
            gradient_r = CalculateGradient('r');//求右线斜率
            if(((gradient_l * gradient_r < 0) && gradient_l < 0.5 && gradient_l > 0.2 && gradient_r < -0.2 && gradient_r > -0.5)||(dl1a_distance_mm < 600))
            {
                gpio_set_level(BEER,1);//开启蜂鸣器
                speed_type=kNormalSpeed;//关闭速度决策
                base_speed = 58;//降速
                last_track_mode = track_mode;//切换成电磁循迹
                track_mode = kTrackADC;
                slope_type = kSlopeUp;//切换下一个状态
                encoder_dis_flag = 1;//开启编码器积分
            }
            break;
        }
        case kSlopeUp:
        {
            dl1a_get_distance();
            if(dl1a_distance_mm > 650)
                slope_type = kSlopeEnd;
            break;
        }
        case kSlopeEnd:
        {
            if((dis >= 2500) || (NORMAL_M >= 4095))//编码器积分1.2m，跳出坡道状态
            {
                encoder_dis_flag = 0;//编码器积分标志位清零
                base_speed = original_speed;
                last_track_mode = track_mode;//改成图像循迹
                speed_type=kImageSpeed;//启动速度决策
                track_mode = kTrackImage;
                slope_type = kSlopeBegin;//复位状态机
                gpio_set_level(BEER,0);//关闭蜂鸣器
                return 1;
            }
            break;
        }
        default:break;
    }
    return 0;
}
/***********************************************
* @brief : 坡道计算边线斜率
* @param : flag:选择左线还是右线
* @return: gradient:边线斜率
* @date  : 2023.4.21
* @author: L
************************************************/
float CalculateGradient(uint8 lr_flag)
{
    switch(lr_flag)
    {
        case 'l'://求左线斜率
        {
            if(per_l_line_count < 160) return 0;//边线很长，返回0
            float x = 0,y = 0,xx = 0,xy = 0;
            float x_ave = 0,y_ave = 0,xx_ave = 0,xy_ave = 0;
            float gradient = 0;
            for(int i = 0;i < 160;i++)
            {
                x += f_left_line1[i].Y;
                y += f_left_line1[i].X;
                xx += f_left_line1[i].Y * f_left_line1[i].Y;
                xy += f_left_line1[i].Y * f_left_line1[i].X;
            }
            x_ave = x / 160.0;
            y_ave = y / 160.0;
            xx_ave = xx / 160.0;
            xy_ave = xy / 160.0;

            gradient = (xy_ave - x_ave * y_ave)/(xx_ave - x_ave * x_ave);
            return gradient;
            break;
        }
        case 'r'://求右线斜率
        {
            if(per_r_line_count < 160) return 0;//边线很长，返回0
            float x = 0,y = 0,xx = 0,xy = 0;
            float x_ave = 0,y_ave = 0,xx_ave = 0,xy_ave = 0;
            float gradient = 0;
            for(int i = 0;i < 160;i++)
            {
                x += f_right_line1[i].Y;
                y += f_right_line1[i].X;
                xx += f_right_line1[i].Y * f_right_line1[i].Y;
                xy += f_right_line1[i].Y * f_right_line1[i].X;
            }
            x_ave = x / 160.0;
            y_ave = y / 160.0;
            xx_ave = xx / 160.0;
            xy_ave = xy / 160.0;

            gradient = (xy_ave - x_ave * y_ave)/(xx_ave - x_ave * x_ave);
            return gradient;
            break;
        }
        default:break;
    }
    return 0;
}
