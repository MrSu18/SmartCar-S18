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
#include "pid.h"
#include "adc.h"
#include "bluetooth.h"
#include "filter.h"


//编码器使用的引脚
#define ENCODER_RIGHT   (TIM6_ENCODER)
#define ENCODER_RIGHT_A (TIM6_ENCODER_CH1_P20_3)
#define ENCODER_RIGHT_B (TIM6_ENCODER_CH2_P20_0)

#define ENCODER_LEFT   (TIM2_ENCODER)
#define ENCODER_LEFT_A (TIM2_ENCODER_CH1_P33_7)
#define ENCODER_LEFT_B (TIM2_ENCODER_CH2_P33_6)

#define  MOTOR_PWM_MAX      8000                        //电机PWM限幅

#define  MOTOR_SPEED_LIMIT  280                         //速度限幅

#define  MOTOR_SPEED_BASE   120                         //基础速度
//电机PWM引脚
#define  MOTOR_LEFT_1   ATOM0_CH4_P02_4
#define  MOTOR_LEFT_2   ATOM0_CH6_P02_6
#define  MOTOR_RIGHT_1  ATOM0_CH5_P02_5
#define  MOTOR_RIGHT_2  ATOM0_CH7_P02_7
//左右环岛的判断标志位
#define  CIRCLE_LEFT    1
#define  CIRCLE_RIGHT   2

void EncoderInit(void);                                                 //初始化左右两个编码器
void EncoderGetCount(int16* data_left,int16* data_right);               //获取左右两个编码器的值
void MotorInit(void);                                                   //初始化左右两个电机
void MotorSetPWM(int pwm_left,int pwm_right);                           //控制左右电机转速和正反转
void MotorSetTarget(int16* target_left,int16* target_right);            //获取电机的目标速度
void MotorCtrl(void);                                                   //实现整一轮的小车控制
int8 CircleIsland(void);                                                //环岛检测
float EncoderGetDistance(void);                                         //获取一段时间内小车走过的距离

#endif /* CODE_MOTOR_H_ */
