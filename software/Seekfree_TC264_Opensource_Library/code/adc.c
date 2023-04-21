/*
 * adc.c
 *
 *  Created on: 2023��1��5��
 *      Author: L
 */
#include "adc.h"
#include "zf_device_tft180.h"

int16 adc_value[5] = {0};                           //��ȡ��ȡ����ADC��ֵ
float adc_bias = 0;
//����ɨ��ʱ�õ������ֵ����Сֵ
int16 adc_max[5] = {3303,3228,2810,2981,3306};
int16 adc_min[5] = {0,18,4,0,0};

adc_channel_enum my_adc_pin[5]=
{
    ADC_LEFT_1,
    ADC_LEFT_2,
    ADC_MID,
    ADC_RIGHT_2,
    ADC_RIGHT_1,
};                                                  //ADC�������ŵ�����
/***********************************************
* @brief : ��ʼ��5��ADCͨ��
* @param : void
* @return: void
* @date  : 2023.1.5
* @author: L
************************************************/
void ADCInit(void)
{
    adc_init(ADC_LEFT_1,ADC_12BIT);                 //����ߵ��
    adc_init(ADC_LEFT_2,ADC_12BIT);                 //�����ڶ������
    adc_init(ADC_MID,ADC_12BIT);                    //�м���
    adc_init(ADC_RIGHT_1,ADC_12BIT);                //���ұߵ��
    adc_init(ADC_RIGHT_2,ADC_12BIT);                //�����ڶ������
}
/***********************************************
* @brief : ADCת��һ�Σ���ȡ5����е�ֵ
* @param : value:��ȡADCת��һ�ε�ֵ
* @return: void
* @date  : 2023.1.5
* @author: L
************************************************/
void ADCGetValue(int16* value)
{
    for(int8 i=0;i<5;i++)
    {
        *value = adc_convert(my_adc_pin[i]);                            //��ȡADCת����ֵ
        *value = 100*(*value-adc_min[i])/(adc_max[i]-adc_min[i]);       //��һ������
        for(int j=0;j<6;j++)
            *value = KalmanFilter(&kalman_adc,*value);                  //�������˲�
//        tft180_show_int(98, 15*i, *value, 4);
        value++;
    }
}
/***********************************************
* @brief : ��ȺͲ��㷨
* @param : err:ͨ����ȺͲ�����õ���ƫ��
*          flag:�ж�������������е�ƫ��
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
            *err=(float)30*(LM-RM)/(LM+RM);                 //ѭ���õĵ��ƫ��Ĳ�Ⱥͼ���
            break;
        }
        case JUDGE:
        {
            *err = L-R;                                     //�ж�Ԫ���õ�ƫ��
            break;
        }
        default:break;
    }
}
/***********************************************
* @brief : ����ɨ����򣬵õ�ÿ��������ֵ����Сֵ��ͨ�����ڴ�ӡ�����ڹ�һ��
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
        for(int8 i = 0;i<5;i++)                                             //��ȡ���ֵ
        {
            temp = adc_convert(my_adc_pin[i]);
            if(max[i]<temp)
                max[i] = temp;
            if(min[i]>temp)
                min[i] = temp;
        }
        for(int8 i = 0;i<5;i++)                                             //tft��ʾ
        {
            tft180_show_int(0, 15*i, max[i], 4);
            tft180_show_int(100, 15*i, min[i], 4);
        }
        system_delay_ms(100);
    }
}
