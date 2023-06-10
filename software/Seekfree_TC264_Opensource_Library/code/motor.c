/*
 * motor.c
 *
 *  Created on: 2023��1��4��
 *      Author: L
 */
#include "motor.h"
#include "zf_common_headfile.h"
#include "Control.h"
#include "ADRC.h"

int speed_left = 0,speed_right = 0;                                     //�����ֵ�ǰ��������ֵ
int target_left = 0,target_right = 0;                                   //�����ֵ�Ŀ���ٶȵ�ֵ
uint8 c0h0_isr_flag=0,c0h1_isr_flag=0;                                  //0��ͨ��0�ı�־λ 0:û���ж� 1:�ж�
uint16 base_speed = 0;                                                  //�����ٶ�
TrackMode track_mode = kTrackImage;                                     //Ѳ��ģʽ
TrackMode last_track_mode = kTrackImage;                                //��һ��Ѳ��ģʽ
uint8 encoder_dis_flag = 0;                                             //���������ֱ�־λ
float dis = 0;                                                          //����

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

    *data_left = encoder_get_count(ENCODER_LEFT);                            //��ȡ��߱�������ֵ
    *data_right = -encoder_get_count(ENCODER_RIGHT);                         //��ȡ�ұ߱�������ֵ

    *data_left = SecondOrderFilter_L(*data_left);                           //�������һ�׵�ͨ�˲�
    *data_right = SecondOrderFilter_R(*data_right);                         //�ұ�����һ�׵�ͨ�˲�
    //�޷�
    if(*data_left > 255 || *data_left < -255) *data_left = last_data_left;
    if(*data_right > 255 || *data_right < -255) *data_right = last_data_right;

    encoder_clear_count(ENCODER_LEFT);                                      //�����߱���������
    encoder_clear_count(ENCODER_RIGHT);                                     //����ұ߱���������

    //����������
    if(encoder_dis_flag == 1)
    {
        float data_mid = 0;
        data_mid = (float)(*data_left+*data_right)/2;

        dis += ((data_mid/1024.0)*30.0/68.0)*(float)CIRCLE;
    }
    else if(encoder_dis_flag == 0)//������ֵľ���
        dis = 0;
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
    //pwm_init(MOTOR_LEFT_2,12500,0);                   //��ʼ������
    gpio_init(MOTOR_LEFT_2, GPO, 0, GPO_PUSH_PULL);
    pwm_init(MOTOR_RIGHT_1,12500,0);                  //��ʼ���ҵ��
    gpio_init(MOTOR_RIGHT_2, GPO, 0, GPO_PUSH_PULL);
    //pwm_init(MOTOR_RIGHT_2,12500,0);                  //��ʼ���ҵ��
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
    //����
    //pwm_set_duty(MOTOR_LEFT_2,9999);
    gpio_set_level(MOTOR_LEFT_2, 1);
    pwm_set_duty(MOTOR_LEFT_1,5000+(int)(pwm_left/2));
    //�ҵ��
    gpio_set_level(MOTOR_RIGHT_2, 1);
    pwm_set_duty(MOTOR_RIGHT_1,5000-(int)(pwm_right/2));
    //pwm_set_duty(MOTOR_RIGHT_2,9999);
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

    EncoderGetCount(&speed_left,&speed_right);                                      //��ȡ��������ֵ

    pwm_left = PIDSpeed(speed_left,target_left,&speedpid_left);                     //��ȡ����������PWM
    pwm_right = PIDSpeed(speed_right,target_right,&speedpid_right);                 //��ȡ�������ҵ��PWM

    //���ٶȻ�������ʱ������ǰ������
    if(speedpid_left.err>100 || speedpid_left.err<-100)
    {
        pwm_left += FeedForwardCtrl(target_left, &speedffc_left);

    }
    if(speedpid_left.err>100 || speedpid_left.err<-100)
    {
        pwm_right += FeedForwardCtrl(target_right, &speedffc_right);
    }

//    Fhan_ADRC(&adrc_controller_l,(float)pwm_left);
//    Fhan_ADRC(&adrc_controller_r,(float)pwm_right);
    MotorSetPWM(pwm_left,pwm_right);                                                //�������һ��ռ�ձȵ�PWM

    c0h0_isr_flag=1;
}
