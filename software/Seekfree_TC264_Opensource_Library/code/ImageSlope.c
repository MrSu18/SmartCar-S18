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
#include "ImageProcess.h"

/***********************************************
* @brief : 坡道状态机
* @param : void
* @return: 0:不是坡道
*          1:识别到坡道
* @date  : 2023.7.5
* @author: 刘骏帆
************************************************/
uint8 SlopeIdentify(void)
{
    static uint8 status=0;//坡道状态转移变量
    tft180_show_uint(120, 100, status, 2);
    switch(status)
    {
        case 0://开启陀螺仪检测上坡
            StartIntegralAngle_Y(30);//检测上坡
            status=1;
            break;
        case 1://识别坡道
            //tof检测坡道
            dl1a_get_distance();
            //图像检测坡道
            float gradient_l = 0;
            float gradient_r = 0;
            gradient_l = CalculateGradient('l');//求左线斜率
            gradient_r = CalculateGradient('r');//求右线斜率
            //正常情况是图像和tof可以检测到坡道
            if(((gradient_l * gradient_r < 0) && gradient_l < 0.5 && gradient_l > 0.2 && gradient_r < -0.2 && gradient_r > -0.5)||(dl1a_distance_mm < 600)||(icm_angle_y_flag==1))
            {
                gpio_set_level(BEER,1);//开启蜂鸣器
                speed_type=kNormalSpeed;//关闭速度决策
                base_speed = process_property[process_status_cnt].min_speed;//降速
                track_mode = kTrackADC;//切换成电磁循迹
                StartIntegralAngle_Y(-30);//检测下坡
                status=2;//跳转状态到检测下坡状态
            }
            break;
        case 2://检测下坡
            if(icm_angle_y_flag==1)
            {
                encoder_dis_flag = 1;//开启编码器积分，出电磁循迹
                base_speed=original_speed;//恢复速度，但是循迹还是电磁
                status=3;
            }
            break;
        case 3://坡道结束
            if(dis>500)//50cm的下坡
            {
                track_mode=kTrackImage;//切换回图像
                base_speed=original_speed;//恢复速度
                speed_type=kImageSpeed;//开启速度决策
                gpio_set_level(BEER,0);
                status=0;
                return 1;
            }
            break;
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
