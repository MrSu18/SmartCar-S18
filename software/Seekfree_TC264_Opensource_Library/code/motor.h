/*
 * motor.h
 *
 *  Created on: 2023��1��4��
 *      Author: L
 */

#ifndef CODE_MOTOR_H_
#define CODE_MOTOR_H_

#include "zf_driver_pwm.h"
#include "zf_driver_encoder.h"
#include "zf_driver_pit.h"
#include "isr_config.h"
#include "pid.h"
#include "adc.h"
#include "bluetooth.h"
#include "filter.h"

//������ʹ�õ�����
#define ENCODER_RIGHT   (TIM6_ENCODER)
#define ENCODER_RIGHT_A (TIM6_ENCODER_CH1_P20_3)
#define ENCODER_RIGHT_B (TIM6_ENCODER_CH2_P20_0)

#define ENCODER_LEFT   (TIM2_ENCODER)
#define ENCODER_LEFT_A (TIM2_ENCODER_CH1_P33_7)
#define ENCODER_LEFT_B (TIM2_ENCODER_CH2_P33_6)

#define  MOTOR_PWM_MAX      8000                        //���PWM�޷�

#define CIRCLE          3.1415926*65                    //�����ܳ�

//���PWM����
#define  MOTOR_LEFT_1   ATOM0_CH4_P02_4
#define  MOTOR_LEFT_2   ATOM0_CH6_P02_6
#define  MOTOR_RIGHT_1  ATOM0_CH5_P02_5
#define  MOTOR_RIGHT_2  ATOM0_CH7_P02_7
//���һ������жϱ�־λ
#define  CIRCLE_LEFT    1
#define  CIRCLE_RIGHT   2

typedef enum TrackMode
{
    kTrackImage = 0,
    kTrackADC,
}TrackMode;

extern int speed_left,speed_right;
extern int target_left,target_right;
extern int target_left_1,target_right_1;
extern uint8 c0h0_isr_flag,c0h1_isr_flag;
extern uint8 encoder_dis_flag;
extern uint16 base_speed;
extern TrackMode track_mode;
extern TrackMode last_track_mode;
extern float dis;

void EncoderInit(void);                                                 //��ʼ����������������
float EncoderGetCount(int* data_left,int* data_right);                   //��ȡ����������������ֵ
void MotorInit(void);                                                   //��ʼ�������������
void MotorSetPWM(int pwm_left,int pwm_right);                           //�������ҵ��ת�ٺ�����ת
void MotorCtrl(void);                                                   //ʵ����һ�ֵ�С������

#endif /* CODE_MOTOR_H_ */
