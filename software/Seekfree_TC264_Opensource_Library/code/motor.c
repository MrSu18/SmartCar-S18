/*
 * motor.c
 *
 *  Created on: 2023年1月4日
 *      Author: L
 */
#include "motor.h"
#include "zf_common_headfile.h"

int speed_left = 0,speed_right = 0;                                     //左右轮当前编码器的值
int target_left = 0,target_right = 0;                                   //左右轮的目标速度的值
int target_left_1 = 0,target_right_1 = 0;
uint8 c0h0_isr_flag=0,c0h1_isr_flag=0;                                  //0核通道0的标志位 0:没进中断 1:中断
uint16 base_speed = 0;
TrackMode track_mode = kTrackImage;
TrackMode last_track_mode = kTrackImage;

/***********************************************
* @brief : 初始化左右两个编码器
* @param : void
* @return: void
* @date  : 2023.1.2
* @author: L
************************************************/
void EncoderInit(void)
{
    encoder_quad_init(ENCODER_RIGHT,ENCODER_RIGHT_A,ENCODER_RIGHT_B);       //初始化右边编码器模块
    encoder_quad_init(ENCODER_LEFT,ENCODER_LEFT_A,ENCODER_LEFT_B);          //初始化左边编码器模块

}
/***********************************************
* @brief : 获取左右两个编码器的值
* @param : data_left:存取左编码器的值
*          data_right:存取右编码器的值
* @return: void
* @date  : 2023.1.2
* @author: L
************************************************/
void EncoderGetCount(int* data_left,int* data_right)
{
    int last_data_left = *data_left,last_data_right = *data_right;

    *data_left = -encoder_get_count(ENCODER_LEFT);                          //获取左边编码器的值
    *data_right = -encoder_get_count(ENCODER_RIGHT);                         //获取右边编码器的值

    *data_left = SecondOrderFilter_L(*data_left);                           //左编码器一阶低通滤波
    *data_right = SecondOrderFilter_R(*data_right);                         //右编码器一阶低通滤波

    if(*data_left > 255 || *data_left < -255) *data_left = last_data_left;
    if(*data_right > 255 || *data_right < -255) *data_right = last_data_right;

    encoder_clear_count(ENCODER_LEFT);                                      //清空左边编码器计数
    encoder_clear_count(ENCODER_RIGHT);                                     //清空右边编码器计数

}
/***********************************************
* @brief : 初始化左右两个电机
* @param : void
* @return: void
* @date  : 2023.1.5
* @author: L
************************************************/
void MotorInit(void)
{
    pwm_init(MOTOR_LEFT_1,12500,0);                   //初始化左电机
    pwm_init(MOTOR_LEFT_2,12500,0);                   //初始化左电机
    pwm_init(MOTOR_RIGHT_1,12500,0);                  //初始化右电机
    pwm_init(MOTOR_RIGHT_2,12500,0);                  //初始化右电机
}
/***********************************************
* @brief : 控制左右两个电机的转速和正反转,采用H桥的单极模式
* @param : pwm_left:给左电机的pwm大小，控制左电机转速
*          pwm_right:给右电机的pwm大小，控制右电机转速
* @return: void
* @date  : 2023.1.5
* @author: L
************************************************/
void MotorSetPWM(int pwm_left,int pwm_right)
{
    //对输入电机的pwm进行限幅
    if(pwm_left>MOTOR_PWM_MAX)
        pwm_left = MOTOR_PWM_MAX;
    else if(pwm_left<-MOTOR_PWM_MAX)
        pwm_left = -MOTOR_PWM_MAX;
    if(pwm_right>MOTOR_PWM_MAX)
        pwm_right = MOTOR_PWM_MAX;
    else if(pwm_right<-MOTOR_PWM_MAX)
        pwm_right = -MOTOR_PWM_MAX;
    //控制电机正反转和转速
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
* @brief : 增量式PI控制电机转速
* @param : void
* @return: void
* @date  : 2023.1.17
* @author: L
************************************************/
void MotorCtrl(void)
{
    int pwm_left = 0,pwm_right = 0;                                                 //左右电机PWM
    int pwm_left_1 = 0,pwm_right_1 = 0;

    EncoderGetCount(&speed_left,&speed_right);                                      //获取编码器的值

    if(track_mode == kTrackImage)                                                   //当前为摄像头循迹
    {
        pwm_left = PIDSpeed(speed_left,target_left,&speedpid_left);                 //获取赛道上左电机PWM
        pwm_right = PIDSpeed(speed_right,target_right,&speedpid_right);             //获取赛道上右电机PWM
//
        pwm_left_1 = PIDSpeed(speed_left,target_left_1,&speedpid_left_1);
        pwm_right_1 = PIDSpeed(speed_right,target_right_1,&speedpid_right_1);

        MotorSetPWM(pwm_left,pwm_right);
    }
    else if(track_mode == kTrackADC)                                                //当前为电磁循迹
    {
        pwm_left_1 = PIDSpeed(speed_left,target_left_1,&speedpid_left_1);             //获取蓝布上左电机PWM
        pwm_right_1 = PIDSpeed(speed_right,target_right_1,&speedpid_right_1);         //获取蓝布上右电机PWM

        MotorSetPWM(pwm_left_1,pwm_right_1);
    }

    c0h0_isr_flag=1;
}
/***********************************************
* @brief : 计算一段时间走过的路程
* @param : 1024线编码器，编码器齿轮齿数30，车模齿轮齿数68，车轮直径64mm
* @return: dis:一段时间走过的路程，单位为mm
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

