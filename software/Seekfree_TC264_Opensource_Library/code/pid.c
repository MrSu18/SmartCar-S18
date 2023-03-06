/*
 * pid.c
 *
 *  Created on: 2023��1��9��
 *      Author: L
 */
#include "pid.h"

PID speedpid_left;                          //�����ٶȻ�PID
PID speedpid_right;                         //�����ٶȻ�PID
PID turnpid;                                //ת��PID
float Bias = 0;

/***********************************************
* @brief : ��ʼ��PID����
* @param : pid:PID�����Ľṹ��ָ��
*          P:����ϵ��
*          I:������
*          D:΢����
* @return: void
* @date  : 2023.1.9
* @author: L
************************************************/
void PIDInit(PID* pid,float P,float I,float D)
{
    pid->P = P;
    pid->I = I;
    pid->D = D;
    pid->err = 0;
    pid->last_err = 0;
    pid->out = 0;
}
/***********************************************
* @brief : ����ٶȻ�����ʽPI����
* @param : encoder_data:��������ֵ
*          target_data:Ŀ��ֵ
*          pid:����Ȼ���PID
* @return: out:�������PWM��С
* @date  : 2023.1.17
* @author: L
************************************************/
int PIDSpeed(int16 encoder_speed,int16 target_speed,PID* pid)
{
    pid->err = target_speed-encoder_speed;                                  //����ƫ��

    pid->out += (int)(pid->P*(pid->err-pid->last_err)+pid->I*pid->err);
    pid->last_err = pid->err;                                               //������һ�ε�ֵ

    return pid->out;
}
/***********************************************
* @brief : ת��λ��ʽPD����
* @param : pid:ת�򻷵�PID
* @return: out:������Ŀ���ٶȵı仯��
* @date  : 2023.3.2
* @author: L
************************************************/
void PIDTurn(int16* target_left,int16* target_right,PID* pid)
{
//    //��ȡ��ʱƫ��
//    ADCGetValue(adc_value);
//    ChaBiHe(&pid->err,TRACK);
    pid->err = Bias;

    pid->out = (int)(pid->P*pid->err+pid->D*(pid->err-pid->last_err));
    pid->last_err = pid->err;                                               //������һ�ε�ֵ

    if(pid->out>0)//��ת
    {
        *target_left = MOTOR_SPEED_BASE - pid->out;
        *target_right = MOTOR_SPEED_BASE;
    }
    else
    {
        *target_left = MOTOR_SPEED_BASE;
        *target_right = MOTOR_SPEED_BASE + pid->out;
    }
}
