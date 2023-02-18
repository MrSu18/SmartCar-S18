/*
 * motor.c
 *
 *  Created on: 2023年1月4日
 *      Author: L
 */
#include "motor.h"
#include "zf_common_headfile.h"

//extern PID speedpid_left;
//extern PID speedpid_right;
//extern PID turnpid;
//extern int16 adc_value[5];
int16 last_data_l = 0,last_data_r = 0;
int8 circle_flag = 0;

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
void EncoderGetCount(int16* data_left,int16* data_right)
{
    *data_left = encoder_get_count(ENCODER_LEFT);                           //获取左边编码器的值
    *data_right = -encoder_get_count(ENCODER_RIGHT);                        //获取右边编码器的值

    *data_left = SecondOrderFilter_L(*data_left);                           //左编码器一阶低通滤波
    *data_right = SecondOrderFilter_R(*data_right);                         //右编码器一阶低通滤波

    encoder_clear_count(ENCODER_LEFT);                                      //清空左边编码器计数
    encoder_clear_count(ENCODER_RIGHT);                                     //清空右边编码器计数

    //printf("%d      %d\n",*data_left,*data_right);
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
///***********************************************
//* @brief : 获取目标速度
//* @param : target_left:左边电机目标速度
//*          target_right:右边电机目标速度
//* @return: void
//* @date  : 2023.1.15
//* @author: L
//************************************************/
//void MotorSetTarget(int16* target_left,int16* target_right)
//{
//    int err = 0;
//    err = PIDTurn(&turnpid);
////    ChaBiHe(&err,TRACK);
//
//    *target_left = MOTOR_SPEED_BASE-err;
//    *target_right = MOTOR_SPEED_BASE+err;
//
//    if(*target_left>MOTOR_SPEED_BASE+MOTOR_SPEED_LIMIT)
//        *target_left = MOTOR_SPEED_BASE+MOTOR_SPEED_LIMIT;
//    else if(*target_left<MOTOR_SPEED_BASE-MOTOR_SPEED_LIMIT)
//        *target_left = MOTOR_SPEED_BASE-MOTOR_SPEED_LIMIT;
//    if(*target_right>MOTOR_SPEED_BASE+MOTOR_SPEED_LIMIT)
//        *target_right = MOTOR_SPEED_BASE+MOTOR_SPEED_LIMIT;
//    else if(*target_right<MOTOR_SPEED_BASE-MOTOR_SPEED_LIMIT)
//        *target_right = MOTOR_SPEED_BASE-MOTOR_SPEED_LIMIT;
//
////    printf("%d      ",*target_left);
////    printf("%d\n",*target_right);
//}
///***********************************************
//* @brief : 增量式PI控制电机转速
//* @param : void
//* @return: void
//* @date  : 2023.1.17
//* @author: L
//************************************************/
//void MotorCtrl(void)
//{
//    int pwm_left = 0,pwm_right = 0;
//    int16 speed_left = 0,speed_right = 0;
//    int16 target_left = 0,target_right = 0;
//
//    EncoderGetCount(&speed_left,&speed_right);
//    MotorSetTarget(&target_left,&target_right);
//
////    printf("%d,%d\n",target_left,speed_left);
//    if(circle_flag!=0)
//    {
//        speed_left = (speed_left+last_data_l)/2;
//        speed_right = (speed_right+last_data_r)/2;
//        circle_flag = 0;
//    }
//
//    pwm_left = PIDSpeedLeft(speed_left,target_left,&speedpid_left);
//    pwm_right = PIDSpeedRight(speed_right,target_right,&speedpid_right);
//
//    MotorSetPWM(pwm_left,pwm_right);
//}
///***********************************************
//* @brief : 计算一段时间走过的路程
//* @param : 1024线编码器，编码器齿轮齿数30，车模齿轮齿数68，车轮直径64mm
//* @return: dis:一段时间走过的路程，单位为mm
//* @date  : 2023.1.31
//* @author: L
//************************************************/
//float EncoderGetDistance(void)
//{
//    float dis = 0.0,data_mid = 0.0,circle = 0.0;
//    int16 data_left = 0,data_right = 0;
//
//    EncoderGetCount(&data_left,&data_right);
//    data_mid = (data_left+data_right)/2;
//
//    circle = PI*64;
//    dis = ((data_mid/1024)*30/68)*circle;
//    return dis;
//}
///***********************************************
//* @brief : 判断是否为环岛
//* @param : void
//* @return: void
//* @date  : 2023.2.1
//* @author: L
//************************************************/
//int8 CircleIsland(void)
//{
//    float err = 0.0,dis = 0.0;
//
//    ADCGetValue(adc_value);
//    ChaBiHe(&err,JUDGE);
//    if(err>=70)
//    {
//        EncoderGetCount(&last_data_l,&last_data_r);
//        circle_flag = 1;
//    }
//    else if(err<=-70)
//    {
//        EncoderGetCount(&last_data_l,&last_data_r);
//        circle_flag = 2;
//    }
//    else return 0;
//
//    MotorSetPWM(1500,1500);
//    pit_disable(CCU60_CH0);
//
//    while(1)
//    {
//        if(dis>400)
//        {
//        switch(circle_flag)
//            {
//                case CIRCLE_LEFT:
//                {
//                    MotorSetPWM(-2000,2000);
//                    system_delay_ms(300);
//                    break;
//                }
//                case CIRCLE_RIGHT:
//                {
//                    MotorSetPWM(2000,-2000);
//                    system_delay_ms(300);
//                    break;
//                }
//                default:break;
//             }
//            break;
//        }
//        else
//        {
//            dis += EncoderGetDistance();
//            system_delay_ms(5);
//        }
//    }
//    pit_enable(CCU60_CH0);
//    return 1;
//}
