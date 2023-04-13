/*
 * FuzzyPID.c
 *
 *  Created on: 2023��4��13��
 *      Author: L
 */
#include "zf_common_headfile.h"
#include "pid.h"
#include "motor.h"
#include "FuzzyPID.h"
#include "zf_device_tft180.h"

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
* @brief : �������ƫ���ƫ��ı仯��ģ����������Kp��Kd��������ֵ
* @param : E:ƫ��
*          EC;ƫ��ı仯��
*          memership[4]:kp��kd��������ֵ
*          index_E[2]:ƫ���������
*          index_EC[2]:ƫ��仯����������
* @return: void
* @date  : 2023.4.9
* @author: L
************************************************/
float range[7] = { NB,NM,NS,ZO,PS,PM,PB };
void Fuzzification(float E,float EC,float memership[4],int index_E[2],int index_EC[2])
{
    float memership_E[2] = { 0 };//E��������ֵ
    float memership_EC[2] = { 0 };//EC��������ֵ
    //E������ȷ��
    if (E >= range[0] && E <= range[6])
    {
        for (int i = 0; i < 7; i++)
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
    else if (E < range[0])
    {
        index_E[0] = 0;
        index_E[1] = -1;
        memership_E[0] = 1;
        memership_E[1] = 0;
    }
    else if (E > range[6])
    {
        index_E[0] = 6;
        index_E[1] = -1;
        memership_E[0] = 1;
        memership_E[1] = 0;
    }
    //EC������ȷ��
    if (EC >= range[0] && EC <= range[6])
    {
        for (int i = 0; i < 7; i++)
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
    else if (EC < range[0])
    {
        index_EC[0] = 0;
        index_EC[1] = -1;
        memership_EC[0] = 1;
        memership_EC[1] = 0;
    }
    else if (EC > range[6])
    {
        index_EC[0] = 6;
        index_EC[1] = -1;
        memership_EC[0] = 1;
        memership_EC[1] = 0;
    }
    //����KP��KD��������ֵ
    memership[0] = memership_E[0] * memership_EC[0];
    memership[1] = memership_E[0] * memership_EC[1];
    memership[2] = memership_E[1] * memership_EC[0];
    memership[3] = memership_E[1] * memership_EC[1];
}
/***********************************************
* @brief : �Եõ���KP��KD�����Ƚ��н�ģ�����õ�KP��KD���������ж�Ӧ��ֵ
* @param : qE:ƫ��ӳ��������[-3,3]��ֵ
*          EC:ƫ��ı仯��ӳ��������[-3,3]��ֵ
* @return: void
* @date  : 2023.4.10
* @author: L
************************************************/
void SoluteFuzzy(float qE,float qEC)
{
    int index_E[2] = { 0 };//E������������
    int index_EC[2] = { 0 };//EC������������
    float memership[4] = { 0 };
    Fuzzification(qE, qEC, memership, index_E, index_EC);

    for (int i = 0; i < 2; i++)
    {
        if (index_E[i] == -1) continue;
        for (int j = 0; j < 2; j++)
        {
            if (index_EC[i] == -1) continue;
            qdetail_Kp += memership[i + j] * KPFuzzyRule[index_E[i]][index_EC[j]];
            qdetail_Kd += memership[i + j] * KDFuzzyRule[index_E[i]][index_EC[j]];
        }
    }
}
/***********************************************
* @brief : �������е�KP��KD���з�ӳ�䣬�õ�ʵ�ʵ�ֵ��������PID���㹫ʽ�õ�ƫ��
* @param : void
* @return: void
* @date  : 2023.4.10
* @author: L
************************************************/
void FuzzyPID(void)
{
    turnpid_image.err = image_bias;
    float EC = turnpid_image.err - turnpid_image.last_err;
//    tft180_show_float(0, 0, image_bias, 2, 3);
//    tft180_show_float(0, 10, EC, 3, 3);

    float qE = Quantization(E_MAX, E_MIN, turnpid_image.err);
    float qEC = Quantization(EC_MAX, EC_MIN, EC);
    SoluteFuzzy(qE, qEC);

    //detail_Kp = InverseQuantization(KP_MAX, KP_MIN, qdetail_Kp);
    //detail_Kd = InverseQuantization(KD_MAX, KD_MIN, qdetail_Kd);
    //qdetail_Kp = 0;
    //qdetail_Kd = 0;
    //turnpid_image.P += detail_Kp;
    //turnpid_image.D += detail_Kd;

    //if (turnpid_image.P > 25) turnpid_image.P = 25;
    //else if (turnpid_image.P < -25) turnpid_image.P = -25;
    //if (turnpid_image.D > 5) turnpid_image.D = 5;
    //else if (turnpid_image.D < -5) turnpid_image.D = -5;

    turnpid_image.P = InverseQuantization(KP_MAX, KP_MIN, qdetail_Kp);
    turnpid_image.D = InverseQuantization(KD_MAX, KD_MIN, qdetail_Kd);
    qdetail_Kp = 0;
    qdetail_Kd = 0;

    turnpid_image.out = (int)(turnpid_image.P * turnpid_image.err + turnpid_image.D * EC);
    turnpid_image.last_err = turnpid_image.err;

    //target_left = base_speed - turnpid_image.out;
    //target_right = base_speed + turnpid_image.out;
    if (turnpid_image.out > 0)//��ת
    {
        target_left = base_speed - turnpid_image.out;
        target_right = base_speed;
    }
    else
    {
        target_left = base_speed;
        target_right = base_speed + turnpid_image.out;
    }
}
/***********************************************
* @brief : ��һ��ֵӳ�䵽����[-3��3]��
* @param : max:��ֵ��ȡ�������ֵ
*          min:��ֵ��ȡ������Сֵ
* @return: value:ӳ����ֵ
* @date  : 2023.4.10
* @author: L
************************************************/
float Quantization(float max, float min, float x)
{
    float value = 6.0 * (x - min) / (max - min) - 3;

    return value;
}
/***********************************************
* @brief : ����������[-3��3]�е�ֵ��ӳ�䵽ʵ�ʵ�ֵ
* @param : max:��ֵ��ȡ�������ֵ
*          min:��ֵ��ȡ������Сֵ
* @return: value:��ӳ����ʵ��ֵ
* @date  : 2023.4.10
* @author: L
************************************************/
float InverseQuantization(float max, float min, float x)
{
    float value = (max - min) * (x + 3) / 6 + min;

    return value;
}