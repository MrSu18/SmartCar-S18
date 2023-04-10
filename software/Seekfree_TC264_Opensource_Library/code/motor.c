/*
 * motor.c
 *
 *  Created on: 2023��1��4��
 *      Author: L
 */
#include "motor.h"
#include "zf_common_headfile.h"

int speed_left = 0,speed_right = 0;                                     //�����ֵ�ǰ��������ֵ
int target_left = 0,target_right = 0;                                   //�����ֵ�Ŀ���ٶȵ�ֵ
int target_left_1 = 0,target_right_1 = 0;
uint8 c0h0_isr_flag=0,c0h1_isr_flag=0;                                  //0��ͨ��0�ı�־λ 0:û���ж� 1:�ж�
uint16 base_speed = 0;
TrackMode track_mode = kTrackImage;
TrackMode last_track_mode = kTrackImage;

/***********************************************
* @brief : ��ʼ����������������
* @param : void
* @return: void
* @date  : 2023.1.2
* @author: L
************************************************/
void EncoderInit(void)
{
    encoder_quad_init(ENCODER_RIGHT,ENCODER_RIGHT_A,ENCODER_RIGHT_B);       //��ʼ���ұ߱�����ģ��
    encoder_quad_init(ENCODER_LEFT,ENCODER_LEFT_A,ENCODER_LEFT_B);          //��ʼ����߱�����ģ��

}
/***********************************************
* @brief : ��ȡ����������������ֵ
* @param : data_left:��ȡ���������ֵ
*          data_right:��ȡ�ұ�������ֵ
* @return: void
* @date  : 2023.1.2
* @author: L
************************************************/
void EncoderGetCount(int* data_left,int* data_right)
{
    int last_data_left = *data_left,last_data_right = *data_right;

    *data_left = -encoder_get_count(ENCODER_LEFT);                          //��ȡ��߱�������ֵ
    *data_right = -encoder_get_count(ENCODER_RIGHT);                         //��ȡ�ұ߱�������ֵ

    *data_left = SecondOrderFilter_L(*data_left);                           //�������һ�׵�ͨ�˲�
    *data_right = SecondOrderFilter_R(*data_right);                         //�ұ�����һ�׵�ͨ�˲�

    if(*data_left > 255 || *data_left < -255) *data_left = last_data_left;
    if(*data_right > 255 || *data_right < -255) *data_right = last_data_right;

    encoder_clear_count(ENCODER_LEFT);                                      //�����߱���������
    encoder_clear_count(ENCODER_RIGHT);                                     //����ұ߱���������

}
/***********************************************
* @brief : ��ʼ�������������
* @param : void
* @return: void
* @date  : 2023.1.5
* @author: L
************************************************/
void MotorInit(void)
{
    pwm_init(MOTOR_LEFT_1,12500,0);                   //��ʼ������
    pwm_init(MOTOR_LEFT_2,12500,0);                   //��ʼ������
    pwm_init(MOTOR_RIGHT_1,12500,0);                  //��ʼ���ҵ��
    pwm_init(MOTOR_RIGHT_2,12500,0);                  //��ʼ���ҵ��
}
/***********************************************
* @brief : �����������������ת�ٺ�����ת,����H�ŵĵ���ģʽ
* @param : pwm_left:��������pwm��С����������ת��
*          pwm_right:���ҵ����pwm��С�������ҵ��ת��
* @return: void
* @date  : 2023.1.5
* @author: L
************************************************/
void MotorSetPWM(int pwm_left,int pwm_right)
{
    //����������pwm�����޷�
    if(pwm_left>MOTOR_PWM_MAX)
        pwm_left = MOTOR_PWM_MAX;
    else if(pwm_left<-MOTOR_PWM_MAX)
        pwm_left = -MOTOR_PWM_MAX;
    if(pwm_right>MOTOR_PWM_MAX)
        pwm_right = MOTOR_PWM_MAX;
    else if(pwm_right<-MOTOR_PWM_MAX)
        pwm_right = -MOTOR_PWM_MAX;
    //���Ƶ������ת��ת��
    if(pwm_left>=0)
    {
        pwm_set_duty(MOTOR_LEFT_2,pwm_left);
        pwm_set_duty(MOTOR_LEFT_1,0);
    }
    else
    {
        pwm_set_duty(MOTOR_LEFT_2,0);
        pwm_set_duty(MOTOR_LEFT_1,-pwm_left);
    }
    if(pwm_right>=0)
    {
        pwm_set_duty(MOTOR_RIGHT_1,pwm_right);
        pwm_set_duty(MOTOR_RIGHT_2,0);
    }
    else
    {
        pwm_set_duty(MOTOR_RIGHT_1,0);
        pwm_set_duty(MOTOR_RIGHT_2,-pwm_right);
    }
}
/***********************************************
* @brief : ����ʽPI���Ƶ��ת��
* @param : void
* @return: void
* @date  : 2023.1.17
* @author: L
************************************************/
void MotorCtrl(void)
{
    int pwm_left = 0,pwm_right = 0;                                                 //���ҵ��PWM
    int pwm_left_1 = 0,pwm_right_1 = 0;

    EncoderGetCount(&speed_left,&speed_right);                                      //��ȡ��������ֵ

    if(track_mode == kTrackImage)                                                   //��ǰΪ����ͷѭ��
    {
        pwm_left = PIDSpeed(speed_left,target_left,&speedpid_left);                 //��ȡ����������PWM
        pwm_right = PIDSpeed(speed_right,target_right,&speedpid_right);             //��ȡ�������ҵ��PWM
//
        pwm_left_1 = PIDSpeed(speed_left,target_left_1,&speedpid_left_1);
        pwm_right_1 = PIDSpeed(speed_right,target_right_1,&speedpid_right_1);

        MotorSetPWM(pwm_left,pwm_right);
    }
    else if(track_mode == kTrackADC)                                                //��ǰΪ���ѭ��
    {
        pwm_left_1 = PIDSpeed(speed_left,target_left_1,&speedpid_left_1);             //��ȡ����������PWM
        pwm_right_1 = PIDSpeed(speed_right,target_right_1,&speedpid_right_1);         //��ȡ�������ҵ��PWM

        MotorSetPWM(pwm_left_1,pwm_right_1);
    }

    c0h0_isr_flag=1;
}
/***********************************************
* @brief : ����һ��ʱ���߹���·��
* @param : 1024�߱����������������ֳ���30����ģ���ֳ���68������ֱ��64mm
* @return: dis:һ��ʱ���߹���·�̣���λΪmm
* @date  : 2023.1.31
* @author: L
************************************************/
#define PI 3.14
float EncoderGetDistance(void)
{
    float dis = 0.0,data_mid = 0.0,circle = 0.0;
    int data_left = 0,data_right = 0;

    EncoderGetCount(&data_left,&data_right);
    data_mid = (float)(data_left+data_right)/2;

    circle = PI*64;
    dis = ((data_mid/1024)*30/68)*circle;
    return dis;
}

