/*
 * adc.c
 *
 *  Created on: 2023年1月5日
 *      Author: L
 */
#include "adc.h"
#include "zf_device_tft180.h"

int16 adc_value[5] = {0};                           //存取获取到的ADC的值
float adc_bias = 0;
//赛道扫描时得到的最大值和最小值
int16 adc_max[5] = {3303,3228,2810,2981,3306};
int16 adc_min[5] = {0,18,4,0,0};

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
void ADCGetValue(int16* value)
{
    for(int8 i=0;i<5;i++)
    {
        *value = adc_convert(my_adc_pin[i]);                            //获取ADC转换的值
        *value = 100*(*value-adc_min[i])/(adc_max[i]-adc_min[i]);       //归一化处理
        for(int j=0;j<6;j++)
            *value = KalmanFilter(&kalman_adc,*value);                  //卡尔曼滤波
//        tft180_show_int(98, 15*i, *value, 4);
        value++;
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
void ChaBiHe(float* err,int8 flag)
{
    switch(flag)
    {
        case TRACK:
        {
            *err=(float)30*(LM-RM)/(LM+RM);                 //循迹用的电磁偏差的差比和计算
            break;
        }
        case JUDGE:
        {
            *err = L-R;                                     //判断元素用的偏差
            break;
        }
        default:break;
    }
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
