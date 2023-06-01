                            /*
 * adc.h
 *
 *  Created on: 2023年1月5日
 *      Author: L
 */

#ifndef CODE_ADC_H_
#define CODE_ADC_H_

#include "zf_common_headfile.h"
#include "zf_driver_adc.h"
#include "filter.h"

extern int16 adc_value[5];
extern int16 normalvalue[5];

//adc引脚
#define  ADC_LEFT_1    ADC1_CH4_A20
#define  ADC_LEFT_2    ADC1_CH5_A21
#define  ADC_MID       ADC0_CH13_A13
#define  ADC_RIGHT_1   ADC1_CH1_A17
#define  ADC_RIGHT_2   ADC1_CH0_A16
//五个电感值的宏定义
#define L   adc_value[0]
#define LM  adc_value[1]
#define M   adc_value[2]
#define RM  adc_value[3]
#define R   adc_value[4]
//五个电感值归一化后的数据
#define NORMAL_L    normalvalue[0]
#define NORMAL_LM   normalvalue[1]
#define NORMAL_M    normalvalue[2]
#define NORMAL_RM   normalvalue[3]
#define NORMAL_R    normalvalue[4]

#define JUDGE  0                                //求判断元素的偏差
#define TRACK  1                                //求循迹用的偏差

void ADCInit(void);                             //初始化ADC引脚
void ADCGetValue(int16 value[5]);                 //获取ADC转换一次的值
float ChaBiHe(int8 flag);                       //差比和差算法
void ADCScan(void);                             //赛道扫描

#endif /* CODE_ADC_H_ */
