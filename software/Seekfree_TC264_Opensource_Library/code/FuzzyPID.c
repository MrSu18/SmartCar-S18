/*
 * FuzzyPID.c
 *
 *  Created on: 2023年4月13日
 *      Author: L
 */
#include "zf_common_headfile.h"
#include "pid.h"
#include "motor.h"
#include "FuzzyPID.h"
#include "adc.h"
#include "zf_device_tft180.h"
#include "icm20602.h"

float qdetail_Kp = 0,qdetail_Kd = 0;
int8 KPFuzzyRule[7][7] = { {PB,PB,PM,PM,PS,ZO,ZO},
                            {PB,PB,PM,PS,PS,ZO,NS},
                            {PM,PM,PM,PS,ZO,NS,NS},
                            {PM,PM,PS,ZO,NS,NM,NM},
                            {PS,PS,ZO,NS,NS,NM,NM},
                            {PS,ZO,NS,NM,NM,NM,NB},
                            {ZO,ZO,NM,NM,NM,NB,NB} };
int8 KDFuzzyRule[7][7] = { {PS,NS,NB,NB,NB,NM,PS},
                            {PS,NS,NB,NM,NM,NS,ZO},
                            {ZO,NS,NM,NM,NS,NS,ZO},
                            {ZO,NS,NS,NS,NS,NS,ZO},
                            {ZO,ZO,ZO,ZO,ZO,ZO,ZO},
                            {PB,NS,PS,PS,PS,PS,PB},
                            {PB,PM,PM,PM,PS,PS,PB} };
/***********************************************
* @brief : 将输入的偏差和偏差的变化量模糊化，计算Kp和Kd的隶属度值
* @param : E:偏差
*          EC;偏差的变化量
*          memership[4]:kp和kd的隶属度值
*          index_E[2]:偏差的隶属度
*          index_EC[2]:偏差变化量的隶属度
* @return: void
* @date  : 2023.4.9
* @author: L
************************************************/
float range[7] = { NB,NM,NS,ZO,PS,PM,PB };//论域的范围
void Fuzzification(float E,float EC,float memership[4],int index_E[2],int index_EC[2])
{
    float memership_E[2] = { 0 };//E的隶属度值
    float memership_EC[2] = { 0 };//EC的隶属度值
    //E隶属度确定
    if (E > range[0] && E < range[6])
    {
        for (int i = 0; i < 6; i++)
        {
            if (E > range[i] && E < range[i + 1])
            {
                index_E[0] = i;
                index_E[1] = i + 1;
                memership_E[0] = (E - range[i]) / (range[i + 1] - range[i]);
                memership_E[1] = (range[i + 1] - E) / (range[i + 1] - range[i]);
                break;
            }
        }
    }
    else if (E <= range[0])
    {
        index_E[0] = 0;
        index_E[1] = -1;
        memership_E[0] = 1;
        memership_E[1] = 0;
    }
    else if (E >= range[6])
    {
        index_E[0] = 6;
        index_E[1] = -1;
        memership_E[0] = 1;
        memership_E[1] = 0;
    }
    //EC隶属度确定
    if (EC > range[0] && EC < range[6])
    {
        for (int i = 0; i < 6; i++)
        {
            if (EC > range[i] && EC < range[i + 1])
            {
                index_EC[0] = i;
                index_EC[1] = i + 1;
                memership_EC[0] = (EC - range[i]) / (range[i + 1] - range[i]);
                memership_EC[1] = (range[i + 1] - EC) / (range[i + 1] - range[i]);
                break;
            }
        }
    }
    else if (EC <= range[0])
    {
        index_EC[0] = 0;
        index_EC[1] = -1;
        memership_EC[0] = 1;
        memership_EC[1] = 0;
    }
    else if (EC >= range[6])
    {
        index_EC[0] = 6;
        index_EC[1] = -1;
        memership_EC[0] = 1;
        memership_EC[1] = 0;
    }
    //计算KP和KD的隶属度值
    memership[0] = memership_E[0] * memership_EC[0];
    memership[1] = memership_E[0] * memership_EC[1];
    memership[2] = memership_E[1] * memership_EC[0];
    memership[3] = memership_E[1] * memership_EC[1];
}
/***********************************************
* @brief : 对得到的KP和KD隶属度进行解模糊，得到KP和KD的在论域中对应的值
* @param : qE:偏差映射在论域[-3,3]的值
*          EC:偏差的变化量映射在论域[-3,3]的值
* @return: void
* @date  : 2023.4.10
* @author: L
************************************************/
void SoluteFuzzy(float qE,float qEC)
{
    int index_E[2] = { 0 };//E的隶属度索引
    int index_EC[2] = { 0 };//EC的隶属度索引
    float memership[4] = { 0 };//KP和KD的隶属度值
    Fuzzification(qE, qEC, memership, index_E, index_EC);//模糊处理和模糊推理

    //得到KP和KD在论域中的值
    for (int i = 0; i < 2; i++)
    {
        if (index_E[i] == -1) continue;
        for (int j = 0; j < 2; j++)
        {
            if (index_EC[j] == -1) continue;
            qdetail_Kp += memership[(i << i) + j] * KPFuzzyRule[index_E[i]][index_EC[j]];
            qdetail_Kd += memership[(i << i) + j] * KDFuzzyRule[index_E[i]][index_EC[j]];
        }
    }
}
/***********************************************
* @brief : 对论域中的KP和KD进行反映射，得到实际的值，并代入PID计算公式得到偏差
* @param : void
* @return: void
* @date  : 2023.4.10
* @author: L
************************************************/
void FuzzyPID(void)
{
    turnpid_image.err = image_bias;
    float EC = turnpid_image.err - turnpid_image.last_err;//偏差的变化量

    float qE = Quantization(E_MAX, E_MIN, turnpid_image.err);//偏差映射到论域
    float qEC = Quantization(EC_MAX, EC_MIN, EC);//偏差的变化量映射到论域
    SoluteFuzzy(qE, qEC);//解模糊，得到KP和KD在论域中的值
    //KP和KD解模糊，得到实际的值
    turnpid_image.P = InverseQuantization(KP_MAX, KP_MIN, qdetail_Kp);
    turnpid_image.D = InverseQuantization(KD_MAX, KD_MIN, qdetail_Kd);
    qdetail_Kp = 0;
    qdetail_Kd = 0;

    turnpid_image.out = (int)(turnpid_image.P * turnpid_image.err + turnpid_image.D * EC);//PID公式计算输出量
    turnpid_image.last_err = turnpid_image.err;//更新上一次偏差

    if (turnpid_image.out > 0)//左转
    {
        target_left = base_speed - (int)(0.9*turnpid_image.out);
        target_right = base_speed + (int)(0.1*turnpid_image.out);
    }
    else
    {
        target_left = base_speed - (int)(0.1*turnpid_image.out);
        target_right = base_speed + (int)(0.9*turnpid_image.out);
    }
}

void FuzzyPID_ADC(void)
{
    turnpid_adc.err = ChaBiHe(TRACK);
    float EC = turnpid_adc.err - turnpid_adc.last_err;

    float qE = Quantization(E_MAX_A, E_MIN_A, turnpid_adc.err);
    float qEC = Quantization(EC_MAX_A, EC_MAX_A, EC);

    SoluteFuzzy(qE, qEC);//解模糊，得到KP和KD在论域中的值
    //KP和KD解模糊，得到实际的值
    turnpid_adc.P = InverseQuantization(KP_MAX_A, KP_MIN_A, qdetail_Kp);
    turnpid_adc.D = InverseQuantization(KD_MAX_A, KD_MIN_A, qdetail_Kd);
    qdetail_Kp = 0;
    qdetail_Kd = 0;

    turnpid_adc.out = (int)(turnpid_adc.P * turnpid_adc.err + turnpid_adc.D * EC);//PID公式计算输出量
    turnpid_adc.last_err = turnpid_adc.err;//更新上一次偏差

    if (turnpid_adc.out > 0)//左转
    {
        target_left_1 = base_speed - turnpid_adc.out;
        target_right_1 = base_speed;
    }
    else
    {
        target_left_1 = base_speed;
        target_right_1 = base_speed + turnpid_adc.out;
    }
}
/***********************************************
* @brief : 将一个值映射到论域[-3，3]中
* @param : max:该值能取到的最大值
*          min:该值能取到的最小值
* @return: value:映射后的值
* @date  : 2023.4.10
* @author: L
************************************************/
float Quantization(float max, float min, float x)
{
    float value = 6.0 * (x - min) / (max - min) - 3;

    return value;
}
/***********************************************
* @brief : 将处于论域[-3，3]中的值反映射到实际的值
* @param : max:该值能取到的最大值
*          min:该值能取到的最小值
* @return: value:反映射后的实际值
* @date  : 2023.4.10
* @author: L
************************************************/
float InverseQuantization(float max, float min, float x)
{
    float value = (max - min) * (x + 3) / 6 + min;

    return value;
}
