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
* @brief : ��ߵ���ٶȻ�����ʽPI����
* @param : encoder_data:���������ֵ
*          target_data:��ߵ�Ŀ��ֵ
*          pid:����ߵ���Ȼ���PID
* @return: out:��������PWM��С
* @date  : 2023.1.17
* @author: L
************************************************/
int PIDSpeedLeft(int16 encoder_speed,int16 target_speed,PID* pid)
{
    pid->err = target_speed-encoder_speed;                                  //����ƫ��

    pid->out += (int)(pid->P*(pid->err-pid->last_err)+pid->I*pid->err);
    pid->last_err = pid->err;                                               //������һ�ε�ֵ

    return pid->out;
}
/***********************************************
* @brief : �ұߵ���ٶȻ�����ʽPI����
* @param : encoder_data:�ұ�������ֵ
*          target_data:�ұߵ�Ŀ��ֵ
*          pid:�ұߵ���ٶȻ���PID
* @return: out:���ҵ����PWM��С
* @date  : 2023.1.17
* @author: L
************************************************/
int PIDSpeedRight(int16 encoder_speed,int16 target_speed,PID* pid)
{
    pid->err = target_speed-encoder_speed;                                      //����ƫ��

    pid->out += (int)(pid->P*(pid->err-pid->last_err)+pid->I*pid->err);
    pid->last_err = pid->err;                                                   //������һ�ε�ֵ

    return pid->out;
}
/***********************************************
* @brief : ת��λ��ʽPD����
* @param : pid:ת�򻷵�PID
* @return: out:������Ŀ���ٶȵı仯��
* @date  : 2023.2.2
* @author: L
************************************************/
int PIDTurn(PID* pid)
{
    //��ȡ��ʱƫ��
    ADCGetValue(adc_value);
    ChaBiHe(&pid->err,TRACK);

    pid->out = pid->P*pid->err+pid->D*(pid->err-pid->last_err);
    pid->last_err = pid->err;                                               //������һ�ε�ֵ

    return pid->out;
}
/***********************************************
* @brief : ������ȡ�������������ٵ��ȶ���ֵ������һ��ʱ���͵�����
* @param : void
* @return: void
* @date  : 2023.1.30
* @author: L
************************************************/
void AutoPID(void)
{
    int temp=0;
    int16 data1=0,data2=0;

    while(1){
    if(KEY1){
        system_delay_ms(1000);
        encoder_clear_count(ENCODER_LEFT);
        encoder_clear_count(ENCODER_RIGHT);
        MotorSetPWM(1500,1500);
        while(1)
        {
            EncoderGetCount(&data1,&data2);
            printf("%d      ",data1);
            printf("%d\n",data2);
            if(temp>=300)
                MotorSetPWM(0,0);
            temp++;
            system_delay_ms(10);
        }
    }
    }
}
