/*
 * adc.c
 *
 *  Created on: 2023年1月5日
 *      Author: L
 */
#include "adc.h"
#include "zf_device_tft180.h"
#include "pid.h"
#include "filter.h"
#include "stdlib.h"
#include "ImageSpecial.h"

int16 adc_value[5] = {0};                           //存取获取到的ADC的值
int16 normalvalue[5] = {0};

//赛道扫描时得到的最大值和最小值
int16 adc_max[5] = {4095,4095,4095,4068,4095};
int16 adc_min[5] = {64,71,74,59,68};

adc_channel_enum my_adc_pin[5]=
{
    ADC_LEFT_1,
    ADC_LEFT_2,
    ADC_MID,
    ADC_RIGHT_2,
    ADC_RIGHT_1,
};                                                  //ADC５个引脚的数组
/***********************************************
* @brief : 初始化5个ADC通道
* @param : void
* @return: void
* @date  : 2023.1.5
* @author: L
************************************************/
void ADCInit(void)
{
    adc_init(ADC_LEFT_1,ADC_12BIT);                 //最左边电感
    adc_init(ADC_LEFT_2,ADC_12BIT);                 //左数第二个电感
    adc_init(ADC_MID,ADC_12BIT);                    //中间电感
    adc_init(ADC_RIGHT_1,ADC_12BIT);                //最右边电感
    adc_init(ADC_RIGHT_2,ADC_12BIT);                //右数第二个电感
}
/***********************************************
* @brief : ADC转换一次，获取5个电感的值
* @param : value:存取ADC转换一次的值
* @return: void
* @date  : 2023.1.5
* @author: L
************************************************/
void ADCGetValue(int16 value[5])
{
    for(int8 i=0;i<5;i++)
    {
        value[i]=adc_convert(my_adc_pin[i]);//获取ADC转换的值
        normalvalue[i] = 100*(value[i]-adc_min[i])/(adc_max[i]-adc_min[i]);//归一化处理
        for(int8 j=0;j<5;j++)
        {
            normalvalue[i] = KalmanFilter(&kalman_adc,normalvalue[i]);//卡尔曼滤波
//            tft180_show_int(98, 15*i, value[i], 4);
        }
    }
}
/***********************************************
* @brief : 差比和差算法
* @param : err:通过差比和差计算后得到的偏差
*          flag:判断是求哪两个电感的偏差
* @return: void
* @date  : 2023.1.26
* @author: L
************************************************/
float ChaBiHe(int8 flag)
{
    float err = 0;
    static float last_adc_err=0;
    switch(flag)
    {
        case TRACK:
        {
            if((NORMAL_L + NORMAL_R) < 10 && fabs(NORMAL_LM - NORMAL_RM) < 10)
                err = turnpid_adc.err;
            else
               err = 20.0*(float)(NORMAL_LM-NORMAL_RM)/(NORMAL_LM+NORMAL_M+NORMAL_RM);
//                tft180_show_float(0,0,err,3,2);
            err=0.9*err+0.1*last_adc_err;
            if(cut_flag==1)
            {
                if(err>8) err=8;
                else if(err<-8) err=-8;
            }
            last_adc_err=err;
            break;
        }
        case JUDGE:
        {
            err = L-R;                                     //判断元素用的偏差
            break;
        }
        default:break;
    }
    return err;
}
/***********************************************
* @brief : 赛道扫描程序，得到每个电感最大值和最小值并通过串口打印，用于归一化
* @param : void
* @return: void
* @date  : 2023.1.11
* @author: L
************************************************/
void ADCScan(void)
{
    int16 max[5] = {0},min[5] = {4095,4095,4095,4095,4095};
    int16 temp = 0;
    while(1){
        for(int8 i = 0;i<5;i++)                                             //获取最大值
        {
            temp = adc_convert(my_adc_pin[i]);
            if(max[i]<temp)
                max[i] = temp;
            if(min[i]>temp)
                min[i] = temp;
        }
        for(int8 i = 0;i<5;i++)                                             //tft显示
        {
            tft180_show_int(0, 15*i, max[i], 4);
            tft180_show_int(100, 15*i, min[i], 4);
        }
        system_delay_ms(100);
    }
}
