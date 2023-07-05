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
#define ENCODER_RIGHT   (TIM5_ENCODER)
#define ENCODER_RIGHT_A (TIM5_ENCODER_CH1_P10_3)
#define ENCODER_RIGHT_B (TIM5_ENCODER_CH2_P10_1)

#define ENCODER_LEFT   (TIM2_ENCODER)
#define ENCODER_LEFT_A (TIM2_ENCODER_CH1_P33_7)
#define ENCODER_LEFT_B (TIM2_ENCODER_CH2_P33_6)

#define  MOTOR_PWM_MAX      9000                        //���PWM�޷�

#define CIRCLE          3.1415926*65                    //�����ܳ�

//���PWM����
#define  MOTOR_LEFT_1   ATOM0_CH5_P02_5
#define  MOTOR_LEFT_2   P02_7//ATOM0_CH7_P02_7
#define  MOTOR_RIGHT_1  ATOM0_CH4_P02_4
#define  MOTOR_RIGHT_2  P02_6//ATOM0_CH6_P02_6

typedef enum TrackMode
{
    kTrackImage = 0,
    kTrackADC,
}TrackMode;//�л�ͼ��͵��Ѳ�ߵĽṹ��

extern int speed_left,speed_right;
extern int target_left,target_right;
extern uint8 encoder_dis_flag;
extern uint16 base_speed;
extern TrackMode track_mode;
extern TrackMode last_track_mode;
extern float dis;

void EncoderInit(void);                                                 //��ʼ����������������
void EncoderGetCount(int* data_left,int* data_right);                   //��ȡ����������������ֵ
void MotorInit(void);                                                   //��ʼ�������������
void MotorSetPWM(int pwm_left,int pwm_right);                           //�������ҵ��ת�ٺ�����ת
void MotorCtrl(void);                                                   //ʵ����һ�ֵ�С������

#endif /* CODE_MOTOR_H_ */
