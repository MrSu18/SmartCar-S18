/*
 * pid.c
 *
 *  Created on: 2023��1��9��
 *      Author: L
 */
#include "pid.h"
#include "ImageTrack.h"
#include "ADRC.h"
#include "motor.h"
#include "adc.h"

PID speedpid_left;                          //���������ٶȻ�PID
PID speedpid_right;                         //���������ٶȻ�PID
//ͼ��ת��PID
PID turnpid_image;//��ת
//���ת��PID
PID turnpid_adc;                            //���ת��PID
//�ٶȻ�ǰ������
FFC speedffc_left;
FFC speedffc_right;
//���ٶȻ�PID
PID gyropid;
int16 real_gyro = 0;//ʵʱ���ٶ�
int16 last_real_gyro = 0;//��һ�ν��ٶ�

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
* @date  : 2023.6.15
* @author: L & ������
************************************************/
int PIDSpeed(int encoder_speed,int target_speed,PID* pid)
{
    float integral_out=0;
    pid->err = (float)(target_speed-encoder_speed);                                  //����ƫ��
    //��������޷�
    integral_out=pid->I*pid->err;
    if(integral_out>pid->I*65)  integral_out=pid->I*65;
    else if(integral_out<-pid->I*65) integral_out=-pid->I*65;
    //PID����
    pid->out += (int)(pid->P*(pid->err-pid->last_err)+integral_out);
    pid->last_err = pid->err;                                                        //������һ�ε�ֵ

    return pid->out;
}
/***********************************************
* @brief : ����ͷת��λ��ʽPD����
* @param : void
* @return: void
* @date  : 2023.7.2
* @author: L & ������
************************************************/
void PIDTurnImage(void)
{
    turnpid_image.err = image_bias;//ͼ��ƫ��
    float EC = turnpid_image.err - turnpid_image.last_err;//ƫ��ı仯��
    Fhan_ADRC(&adrc_controller_errc, EC);//��EC����TD�˲�

    if (turnpid_image.out > 0)//��ת
    {
        turnpid_image.out = (int)(turnpid_image.P * turnpid_image.err + turnpid_image.D * adrc_controller_errc.x1 + gyropid.P*real_gyro);//PID��ʽ���������
    }
    else//��ת
    {
        turnpid_image.out = (int)(12 * turnpid_image.err + turnpid_image.D * adrc_controller_errc.x1 + gyropid.P*real_gyro);//PID��ʽ���������
    }
    turnpid_image.last_err = turnpid_image.err;//������һ��ƫ��
    //*********************˫������ʱת��PID����޷�***************
   if(turnpid_image.out>150)   turnpid_image.out=150;
   else if(turnpid_image.out<-150)   turnpid_image.out=-150;
   //********************************************************

    if (turnpid_image.out > 0)//��ת
    {
        target_left = base_speed  - (int)turnpid_image.out;
        target_right = base_speed + (int)(0.5*turnpid_image.out);
    }
    else//��ת
    {
        target_left = base_speed  - (int)(0.5*turnpid_image.out);
        target_right = base_speed + (int)turnpid_image.out;
    }
}
/***********************************************
* @brief : ���ת��λ��ʽPD����
* @param : void
* @return: void
* @date  : 2023.7.2
* @author: L & ������
************************************************/
void PIDTurnADC(void)
{
    turnpid_adc.err = ChaBiHe(TRACK);//���ƫ��
    float EC = turnpid_adc.err - turnpid_adc.last_err;//ƫ��仯��

    turnpid_adc.out = (int)(turnpid_adc.P * turnpid_adc.err + turnpid_adc.D * EC + gyropid.P*real_gyro);//PID��ʽ���������
    turnpid_adc.last_err = turnpid_adc.err;//������һ��ƫ��

    if (turnpid_adc.out > 0)//��ת
    {
        target_left = base_speed - (int)turnpid_adc.out;
        target_right = base_speed + (int)(0.3*turnpid_adc.out);
    }
    else//��ת
    {
        target_left = base_speed - (int)(0.3*turnpid_adc.out);
        target_right = base_speed + (int)turnpid_adc.out;
    }
}
/***********************************************
* @brief : ��ʼ��ǰ�����Ʋ���
* @param : ffc:ǰ�����ƽṹ��
*          conf_1:
*          conf_2:
*          conf_3:
* @return: void
* @date  : 2023.6.7
* @author: L
************************************************/
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
/***********************************************
* @brief : ǰ������
* @param : target:ǰ����������
*          ffc:ǰ�����ƽṹ��
* @return: ffc->output:ǰ�����������
* @date  : 2023.6.7
* @author: L
************************************************/
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
