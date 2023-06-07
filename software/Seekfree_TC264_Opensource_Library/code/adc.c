/*
 * adc.c
 *
 *  Created on: 2023��1��5��
 *      Author: L
 */
#include "adc.h"
#include "zf_device_tft180.h"
#include "pid.h"
#include "filter.h"
#include "stdlib.h"
#include "ImageSpecial.h"

int16 adc_value[5] = {0};                           //��ȡ��ȡ����ADC��ֵ
int16 normalvalue[5] = {0};

//����ɨ��ʱ�õ������ֵ����Сֵ
int16 adc_max[5] = {4095,4095,4095,4068,4095};
int16 adc_min[5] = {64,71,74,59,68};

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
void ADCGetValue(int16 value[5])
{
    for(int8 i=0;i<5;i++)
    {
        value[i]=adc_convert(my_adc_pin[i]);//��ȡADCת����ֵ
        normalvalue[i] = 100*(value[i]-adc_min[i])/(adc_max[i]-adc_min[i]);//��һ������
        for(int8 j=0;j<5;j++)
        {
            normalvalue[i] = KalmanFilter(&kalman_adc,normalvalue[i]);//�������˲�
//            tft180_show_int(98, 15*i, value[i], 4);
        }
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
            err = L-R;                                     //�ж�Ԫ���õ�ƫ��
            break;
        }
        default:break;
    }
    return err;
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
