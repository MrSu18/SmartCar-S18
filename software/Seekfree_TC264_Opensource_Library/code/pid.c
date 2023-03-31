/*
 * pid.c
 *
 *  Created on: 2023��1��9��
 *      Author: L
 */
#include "pid.h"
#include "ImageTrack.h"

PID speedpid_left;                          //���������ٶȻ�PID
PID speedpid_right;                         //���������ٶȻ�PID
PID speedpid_left_1;                        //���������ٶȻ�PID
PID speedpid_right_1;                       //���������ٶȻ�PID
PID turnpid_image;                          //ͼ��ת��PID
PID turnpid_adc;                            //���ת��PID

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
* @brief : ����ͷת��λ��ʽPD����
* @param : pid:ͼ��ת�򻷵�PID
* @return: out:������Ŀ���ٶȵı仯��
* @date  : 2023.3.2
* @author: L
************************************************/
void PIDTurnImage(int16* target_left,int16* target_right,PID* pid)
{
    pid->err = image_bias;

    pid->out = (int)(pid->P*pid->err+pid->D*(pid->err-pid->last_err));
    pid->last_err = pid->err;                                               //������һ�ε�ֵ

#if 0
    //�ڼ���Ӳ���    ת��PID P:13 D:0 �ٶ�:120 Ԥ���:0.32 �����������٣���ʱ����������
    *target_left = base_speed - pid->out;
    *target_right = base_speed + pid->out;
#else
    //���ּ������ֲ���
    if(pid->out>0)//��ת
    {
        *target_left = base_speed - pid->out;
        *target_right = base_speed;
    }
    else
    {
        *target_left = base_speed;
        *target_right = base_speed + pid->out;
    }
#endif
}
/***********************************************
* @brief : ���ת��λ��ʽPD����
* @param : pid:���ת�򻷵�PID
* @return: out:������Ŀ���ٶȵı仯��
* @date  : 2023.3.16
* @author: L
************************************************/
void PIDTurnADC(int16* target_left,int16* target_right,PID* pid)
{
        //��ȡ��ʱ���ƫ��
        ADCGetValue(adc_value);
        ChaBiHe(&pid->err,TRACK);

        pid->out = (int)(pid->P*pid->err+pid->D*(pid->err-pid->last_err));
        pid->last_err = pid->err;                                               //������һ�ε�ֵ

        if(pid->out>0)//��ת
        {
            *target_left = base_speed - pid->out;
            *target_right = base_speed;
        }
        else
        {
            *target_left = base_speed;
            *target_right = base_speed + pid->out;
        }
}
/***********************************************
* @brief : ��PID�Ĳ�����λ
* @param : void
* @return: void
* @date  : 2023.3.20
* @author: L
************************************************/
void PIDClear(void)
{
    PIDInit(&speedpid_left,136.13,0.68,0);
    PIDInit(&speedpid_right,144.62,0.71,0);
    PIDInit(&speedpid_left_1,108.53,0.78,0);
    PIDInit(&speedpid_right_1,149.41,1.12,0);
    PIDInit(&turnpid_image,25,0,5);
    PIDInit(&turnpid_adc,10,0,0);
}
