                            /*
 * adc.h
 *
 *  Created on: 2023��1��5��
 *      Author: L
 */

#ifndef CODE_ADC_H_
#define CODE_ADC_H_

#include "zf_common_headfile.h"
#include "zf_driver_adc.h"
#include "filter.h"

extern int16 adc_value[5];

//adc����
#define  ADC_LEFT_1   ADC1_CH4_A20
#define  ADC_LEFT_2   ADC1_CH5_A21
#define  ADC_MID      ADC1_CH1_A17
#define  ADC_RIGHT_1  ADC1_CH0_A16
#define  ADC_RIGHT_2  ADC0_CH13_A13
//������ֵ�ĺ궨��
#define L   adc_value[0]
#define LM  adc_value[1]
#define M   adc_value[2]
#define RM  adc_value[3]
#define R   adc_value[4]

#define JUDGE  0
#define TRACK  1

void ADCInit(void);                             //��ʼ��ADC����
void ADCGetValue(int16* value);                 //��ȡADCת��һ�ε�ֵ
void ChaBiHe(float* err,int8 flag);             //��ȺͲ��㷨
void ADCScan(void);                             //����ɨ��

#endif /* CODE_ADC_H_ */
