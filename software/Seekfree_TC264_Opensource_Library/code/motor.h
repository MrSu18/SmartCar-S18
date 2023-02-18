/*
 * motor.h
 *
 *  Created on: 2023年1月4日
 *      Author: L
 */

#ifndef CODE_MOTOR_H_
#define CODE_MOTOR_H_

#include "zf_driver_pwm.h"
#include "zf_driver_encoder.h"
#include "zf_driver_pit.h"
#include "isr_config.h"
//#include "pid.h"
//#include "adc.h"
#include "bluetooth.h"
#include "filter.h"


//编码器使用的引脚
#define ENCODER_RIGHT   (TIM6_ENCOEDER)
#define ENCODER_RIGHT_A (TIM6_ENCOEDER_CH1_P20_3)
#define ENCODER_RIGHT_B (TIM6_ENCOEDER_CH2_P20_0)

#define ENCODER_LEFT   (TIM2_ENCOEDER)
#define ENCODER_LEFT_A (TIM2_ENCOEDER_CH1_P33_7)
#define ENCODER_LEFT_B (TIM2_ENCOEDER_CH2_P33_6)

//电机PWM限幅
#define  MOTOR_PWM_MAX      8000
//速度限幅
#define  MOTOR_SPEED_LIMIT  280
//基础速度
#define  MOTOR_SPEED_BASE   120
//电机PWM引脚
#define  MOTOR_LEFT_1   ATOM0_CH4_P02_4
#define  MOTOR_LEFT_2   ATOM0_CH6_P02_6
#define  MOTOR_RIGHT_1  ATOM0_CH5_P02_5
#define  MOTOR_RIGHT_2  ATOM0_CH7_P02_7

#define  CIRCLE_LEFT    1
#define  CIRCLE_RIGHT   2

void EncoderInit(void);                                                 //初始化左右两个编码器
void EncoderGetCount(int16* data_left,int16* data_right);               //获取左右两个编码器的值
void MotorInit(void);                                                   //初始化左右两个电机
void MotorSetPWM(int pwm_left,int pwm_right);                           //控制左右电机转速和正反转
void MotorSetTarget(int16* target_left,int16* target_right);            //获取电机的目标速度
void MotorCtrl(void);
int8 CircleIsland(void);
float EncoderGetDistance(void);

#endif /* CODE_MOTOR_H_ */
