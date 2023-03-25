/*
 * motor.c
 *
 *  Created on: 2023��1��4��
 *      Author: L
 */
#include "motor.h"
#include "zf_common_headfile.h"
#include "ImageSpecial.h"

int16 last_data_l = 0,last_data_r = 0;
int8 circle_flag = 0;                                   //Բ����־λ��1Ϊ��⵽����
int16 speed_left = 0,speed_right = 0;                   //�����ֵ�ǰ��������ֵ
uint8 c0h0_isr_flag=0;                                  //0��ͨ��0�ı�־λ 0:û���ж� 1:�ж�
uint16 base_speed = 0;
TrackMode track_mode = kTrackImage;
TrackMode last_track_mode = track_mode;

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
void EncoderGetCount(int16* data_left,int16* data_right)
{
    *data_left = -encoder_get_count(ENCODER_LEFT);                          //��ȡ��߱�������ֵ
    *data_right = encoder_get_count(ENCODER_RIGHT);                         //��ȡ�ұ߱�������ֵ

    *data_left = SecondOrderFilter_L(*data_left);                           //�������һ�׵�ͨ�˲�
    *data_right = SecondOrderFilter_R(*data_right);                         //�ұ�����һ�׵�ͨ�˲�

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
    int16 target_left = 0,target_right = 0;                                         //�����ֵ�Ŀ���ٶȵ�ֵ

    EncoderGetCount(&speed_left,&speed_right);                                      //��ȡ��������ֵ

    if(track_mode == kTrackImage)                                                   //��ǰΪ����ͷѭ��
    {
        if(last_track_mode == kTrackADC)                                            //��һ��ѭ��Ϊ���ѭ����λPID����
            PIDClear();

        PIDTurnImage(&target_left,&target_right,&turnpid_image);                    //����ͷ����PID

        pwm_left = PIDSpeed(speed_left,target_left,&speedpid_left);                 //��ȡ����������PWM
        pwm_right = PIDSpeed(speed_right,target_right,&speedpid_right);             //��ȡ�������ҵ��PWM
    }
    else if(track_mode == kTrackADC)                                                //��ǰΪ���ѭ��
    {
        if(last_track_mode == kTrackImage)                                          //��һ��ѭ��Ϊ����ͷѭ����λPID����
            PIDClear();

        PIDTurnADC(&target_left,&target_right,&turnpid_adc);                        //��ŷ���PID

        pwm_left = PIDSpeed(speed_left,target_left,&speedpid_left_1);               //��ȡ����������PWM
        pwm_right = PIDSpeed(speed_right,target_right,&speedpid_right_1);           //��ȡ�������ҵ��PWM
    }

    c0h0_isr_flag=1;
    MotorSetPWM(pwm_left,pwm_right);                                                //������PWMֵ�������
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
    int16 data_left = 0,data_right = 0;

    EncoderGetCount(&data_left,&data_right);
    data_mid = (data_left+data_right)/2;

    circle = PI*64;
    dis = ((data_mid/1024)*30/68)*circle;
    return dis;
}
/***********************************************
* @brief : �ж��Ƿ�Ϊ����
* @param : void
* @return: �ǻ�������1�����Ƿ���0
* @date  : 2023.2.1
* @author: L
************************************************/
int8 CircleIsland(void)
{
    float err = 0.0,dis = 0.0;

    //��ȡ��ʱ���ƫ��
    ADCGetValue(adc_value);
    ChaBiHe(&err,JUDGE);

    //�ж��Ƿ�Ϊ���������󻷵������һ���
    if(err>=70)
    {
        EncoderGetCount(&last_data_l,&last_data_r);
        circle_flag = 1;
    }
    else if(err<=-70)
    {
        EncoderGetCount(&last_data_l,&last_data_r);
        circle_flag = 2;
    }
    else return 0;

    //��⵽Ϊ������ֱ�У��ص���ʱ�ж�
    MotorSetPWM(1500,1500);
    pit_disable(CCU60_CH0);

    //��ֱ��һ�ξ����ǿ�ƴ�ǣ��뻷
    while(1)
    {
        if(dis>400)
        {
        switch(circle_flag)
            {
                case CIRCLE_LEFT:
                {
                    MotorSetPWM(-2000,2000);
                    system_delay_ms(300);
                    break;
                }
                case CIRCLE_RIGHT:
                {
                    MotorSetPWM(2000,-2000);
                    system_delay_ms(300);
                    break;
                }
                default:break;
             }
            break;
        }
        else
        {
            dis += EncoderGetDistance();                //�ۻ��߹���·��
            system_delay_ms(5);
        }
    }
    pit_enable(CCU60_CH0);                              //������ʱ�ж�
    return 1;
}
