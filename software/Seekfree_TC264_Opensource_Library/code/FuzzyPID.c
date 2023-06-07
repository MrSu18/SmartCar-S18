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
#include "adc.h"
#include "zf_device_tft180.h"
#include "icm20602.h"
#include "ADRC.h"


float EC=0;

float qdetail_Kp = 0,qdetail_Kd = 0;                    //kp��kd�������е�ֵ
//KP��ģ�������
int8 KPFuzzyRule_adc[7][7] = {
                            {PB,PB,PM,PM,PS,ZO,ZO},
                            {PB,PB,PM,PS,PS,ZO,NS},
                            {PM,PM,PM,PS,ZO,NS,NS},
                            {PM,PM,PS,ZO,NS,NM,NM},
                            {PS,PS,ZO,NS,NS,NM,NM},
                            {PS,ZO,NS,NM,NM,NM,NB},
                            {ZO,ZO,NM,NM,NM,NB,NB}
                          };
//KD��ģ�������
int8 KDFuzzyRule_adc[7][7] = {
                            {PS,NS,NB,NB,NB,NM,PS},
                            {PS,NS,NB,NM,NM,NS,ZO},
                            {ZO,NS,NM,NM,NS,NS,ZO},
                            {ZO,NS,NS,NS,NS,NS,ZO},
                            {ZO,ZO,ZO,ZO,ZO,ZO,ZO},
                            {PB,NS,PS,PS,PS,PS,PB},
                            {PB,PM,PM,PM,PS,PS,PB}
                         };
int8 KPFuzzyRule_image[7][7] = {
                          {6, 5, 4, 3, 2, 0, 0,},        //   -3
                          {5, 4, 3, 2, 1, 0, 1,},        //   -2
                          {4, 3, 2, 1, 0, 1, 2,},        //   -1
                          {3, 2, 1, 0, 1, 2, 3,},        //    0
                          {2, 1, 0, 1, 2, 3, 4,},        //    1
                          {1, 0, 1, 2, 3, 4, 5,},        //    2
                          {0, 0, 2, 3, 4, 5, 6}          //    3
                        };
int8 KDFuzzyRule_image[7][7] = {
                           {2, 2, 6, 5, 6, 4, 2,},       //   -3
                           {1, 2, 5, 4, 3, 1, 0,},       //   -2
                           {0, 1, 3, 3, 1, 1, 0,},       //   -1
                           {0, 1, 1, 1, 1, 1, 0,},       //    0
                           {0, 0, 0, 0, 0, 0, 0,},       //    1
                           {5, 1, 1, 1, 1, 1, 1,},       //    2
                           {6, 4, 4, 3, 3, 1, 1}
                         };
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
float range[7] = { NB,NM,NS,ZO,PS,PM,PB };//����ķ�Χ
void Fuzzification(float E,float EC,float memership[4],int index_E[2],int index_EC[2])
{
    float memership_E[2] = { 0 };//E��������ֵ
    float memership_EC[2] = { 0 };//EC��������ֵ
    //E������ȷ��
    if (E > range[0] && E < range[6])
    {
        for (int i = 0; i < 6; i++)
        {
            if (E > range[i] && E < range[i + 1])
            {
                //E������ֵ
                index_E[0] = i;
                index_E[1] = i + 1;
                //E��������
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
    //EC������ȷ��
    if (EC > range[0] && EC < range[6])
    {
        for (int i = 0; i < 6; i++)
        {
            if (EC > range[i] && EC < range[i + 1])
            {
                //EC������ֵ
                index_EC[0] = i;
                index_EC[1] = i + 1;
                //EC��������
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
* @date  : 2023.6.7
* @author: L
************************************************/
void SoluteFuzzy(float qE,float qEC,int8 rule_KP[7][7],int8 rule_KD[7][7])
{
    int index_E[2] = { 0 };//E������������
    int index_EC[2] = { 0 };//EC������������
    float memership[4] = { 0 };//KP��KD��������ֵ
    Fuzzification(qE, qEC, memership, index_E, index_EC);//ģ�������ģ������

    //�õ�KP��KD�������е�ֵ
    for (int i = 0; i < 2; i++)
    {
        if (index_E[i] == -1) continue;
        for (int j = 0; j < 2; j++)
        {
            if (index_EC[j] == -1) continue;
            qdetail_Kp += memership[i*(i+1) + j] * rule_KP[index_E[i]][index_EC[j]];//����Kp�������е�ֵ
            qdetail_Kd += memership[i*(i+1) + j] * rule_KD[index_E[i]][index_EC[j]];//����Kd�������е�ֵ
        }
    }
}
/***********************************************
* @brief : �������е�KP��KD���з�ӳ�䣬�õ�ʵ�ʵ�ֵ��������PID���㹫ʽ�õ�ƫ��
* @param : void
* @return: void
* @date  : 2023.6.7
* @author: L & ������
************************************************/
void FuzzyPID(void)
{
    static int   last_turnpid_out=0;
    turnpid_image.err = image_bias;//ͼ��ƫ��
    EC = turnpid_image.err - turnpid_image.last_err;//ƫ��ı仯��
    Fhan_ADRC(&adrc_controller_errc, EC);//��EC����TD�˲�

    turnpid_image.out = (int)(turnpid_image.P * turnpid_image.err + turnpid_image.D * adrc_controller_errc.x1 + 0.00347*real_gyro);//PID��ʽ���������
    turnpid_image.last_err = turnpid_image.err;//������һ��ƫ��

    //ģ��PIDʱ�˲�
//    turnpid_image.out=0.7*turnpid_image.out+0.3*last_turnpid_out;//����˲�
//    last_turnpid_out=turnpid_image.out;

    //*********************ת��PID����޷�***************
   if(turnpid_image.out>200)   turnpid_image.out=200;
   else if(turnpid_image.out<-200)   turnpid_image.out=-200;
   //*********************************************

    if (turnpid_image.out > 0)//��ת
    {
        target_left = base_speed - (int)turnpid_image.out;
        target_right = base_speed+(int)0.3*turnpid_image.out;
    }
    else//��ת
    {
        target_left = base_speed-(int)0.3*turnpid_image.out;
        target_right = base_speed + (int)turnpid_image.out;
    }
}
/***********************************************
* @brief : �������е�KP��KD���з�ӳ�䣬�õ�ʵ�ʵ�ֵ��������PID���㹫ʽ�õ�ƫ��
* @param : void
* @return: void
* @date  : 2023.4.17
* @author: L
************************************************/
void FuzzyPID_ADC(void)
{
    turnpid_adc.err = ChaBiHe(TRACK);//���ƫ��
    float EC = turnpid_adc.err - turnpid_adc.last_err;//ƫ��仯��

    float qE = Quantization(E_MAX_A, E_MIN_A, turnpid_adc.err);//ƫ��ӳ�䵽����
    float qEC = Quantization(EC_MAX_A, EC_MAX_A, EC);//ƫ��ı仯��ӳ�䵽����

    SoluteFuzzy(qE, qEC, KPFuzzyRule_adc, KDFuzzyRule_adc);//��ģ�����õ�KP��KD�������е�ֵ
    //KP��KD��ģ�����õ�ʵ�ʵ�ֵ
    turnpid_adc.P = InverseQuantization(KP_MAX_A, KP_MIN_A, qdetail_Kp);
    turnpid_adc.D = InverseQuantization(KD_MAX_A, KD_MIN_A, qdetail_Kd);
    qdetail_Kp = 0;
    qdetail_Kd = 0;

    turnpid_adc.out = (int)(turnpid_adc.P * turnpid_adc.err + turnpid_adc.D * EC + 0.00347*real_gyro);//PID��ʽ���������
    turnpid_adc.last_err = turnpid_adc.err;//������һ��ƫ��

    if (turnpid_adc.out > 0)//��ת
    {
        target_left = base_speed - (int)turnpid_adc.out;;
        target_right = base_speed + (int)0.3*turnpid_adc.out;
    }
    else//��ת
    {
        target_left = base_speed - (int)0.3*turnpid_adc.out;
        target_right = base_speed + (int)turnpid_adc.out;
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

/***********************************************
* @brief : ��Ϊ����ƫ���趨��ת��P��D
* @param : ��
* @return: ��
* @date  : 2023.6.7
* @author: ������
************************************************/
void GetTurnImagePID(void)
{
    //P
    if(turnpid_image.err>5 || turnpid_image.err<-5)
    {
        turnpid_image.P=20;
    }
    else//ֱ��ƫ���С
    {
        turnpid_image.P=8;
    }
    //D
    turnpid_image.D=6;
}

