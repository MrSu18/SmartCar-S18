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
PID turnpid_image;                          //ͼ��ת��PID
PID turnpid_adc;                            //���ת��PID
FFC speedffc_left;
FFC speedffc_right;
PID gyropid;
int16 real_gyro = 0;
uint8 gyro_flag = 0;

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
int PIDSpeed(int encoder_speed,int target_speed,PID* pid)
{
    pid->err = (float)(target_speed-encoder_speed);                                  //����ƫ��

    pid->out += (int)(pid->P*(pid->err-pid->last_err)+pid->I*pid->err);
    pid->last_err = pid->err;                                                        //������һ�ε�ֵ

    return pid->out;
}
/***********************************************
* @brief : ����ͷת��λ��ʽPD����
* @param : pid:ͼ��ת�򻷵�PID
* @return: out:������Ŀ���ٶȵı仯��
* @date  : 2023.3.2
* @author: L
************************************************/
void PIDTurnImage(int* target_left,int* target_right,PID* pid)
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
void PIDTurnADC(int* target_left,int* target_right,PID* pid)
{
        //��ȡ��ʱ���ƫ��
        ADCGetValue(adc_value);
//        ChaBiHe(&pid->err,TRACK);

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
* @brief : ������ͷPID����ز�����λ
* @param : void
* @return: void
* @date  : 2023.6.7
* @author: L
************************************************/
void PIDClear(void)
{
    PIDInit(&speedpid_left,185.8,0.61,0);
    PIDInit(&speedpid_right,164.8,0.54,0);
    PIDInit(&turnpid_image,13,0,2);
    PIDInit(&turnpid_adc, 0, 0, 0);
}

void FFCInit(FFC* ffc,float conf_1,float conf_2,float conf_3)
{
    ffc->conf_1 = conf_1;
    ffc->conf_2 = conf_2;
    ffc->conf_3 = conf_3;
    ffc->iuput = 0;
    ffc->last_input = 0;
    ffc->pre_input = 0;
    ffc->output = 0;
}

int FeedForwardCtrl(int target,FFC* ffc)
{
    ffc->iuput = target;

    ffc->output = (int)(ffc->conf_1*(ffc->iuput-ffc->last_input)+ffc->conf_2*(ffc->iuput-2*ffc->last_input+ffc->pre_input))+ffc->conf_3;

    ffc->pre_input = ffc->last_input;
    ffc->last_input = ffc->iuput;

    if(ffc->output>5000) ffc->output=5000;
    else if(ffc->output<-5000) ffc->output=-5000;
    return ffc->output;
}
