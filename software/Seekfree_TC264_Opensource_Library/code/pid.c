/*
 * pid.c
 *
 *  Created on: 2023年1月9日
 *      Author: L
 */
#include "pid.h"
#include "ImageTrack.h"
#include "ADRC.h"
#include "motor.h"
#include "adc.h"

PID speedpid_left;                          //赛道左轮速度环PID
PID speedpid_right;                         //赛道右轮速度环PID
//图像转向环PID
PID turnpid_image;//左转
//电磁转向环PID
PID turnpid_adc;                            //电磁转向环PID
//速度环前馈控制
FFC speedffc_left;
FFC speedffc_right;
//角速度环PID
PID gyropid;
int16 real_gyro = 0;//实时角速度
int16 last_real_gyro = 0;//上一次角速度

/***********************************************
* @brief : 初始化PID参数
* @param : pid:PID参数的结构体指针
*          P:比例系数
*          I:积分项
*          D:微分项
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
* @brief : 电机速度环增量式PI控制
* @param : encoder_data:编码器的值
*          target_data:目标值
*          pid:电机度环的PID
* @return: out:给电机的PWM大小
* @date  : 2023.6.15
* @author: L & 刘骏帆
************************************************/
int PIDSpeed(int encoder_speed,int target_speed,PID* pid)
{
    float integral_out=0;
    pid->err = (float)(target_speed-encoder_speed);                                  //计算偏差
    //积分输出限幅
    integral_out=pid->I*pid->err;
    if(integral_out>pid->I*65)  integral_out=pid->I*65;
    else if(integral_out<-pid->I*65) integral_out=-pid->I*65;
    //PID计算
    pid->out += (int)(pid->P*(pid->err-pid->last_err)+integral_out);
    pid->last_err = pid->err;                                                        //保存上一次的值

    return pid->out;
}
/***********************************************
* @brief : 摄像头转向环位置式PD控制
* @param : void
* @return: void
* @date  : 2023.7.2
* @author: L & 刘骏帆
************************************************/
void PIDTurnImage(void)
{
    turnpid_image.err = image_bias;//图像偏差
    float EC = turnpid_image.err - turnpid_image.last_err;//偏差的变化量
    Fhan_ADRC(&adrc_controller_errc, EC);//对EC进行TD滤波

    if (turnpid_image.out > 0)//左转
    {
        turnpid_image.out = (int)(turnpid_image.P * turnpid_image.err + turnpid_image.D * adrc_controller_errc.x1 + gyropid.P*real_gyro);//PID公式计算输出量
    }
    else//右转
    {
        turnpid_image.out = (int)(12 * turnpid_image.err + turnpid_image.D * adrc_controller_errc.x1 + gyropid.P*real_gyro);//PID公式计算输出量
    }
    turnpid_image.last_err = turnpid_image.err;//更新上一次偏差
    //*********************双环串级时转向PID输出限幅***************
   if(turnpid_image.out>150)   turnpid_image.out=150;
   else if(turnpid_image.out<-150)   turnpid_image.out=-150;
   //********************************************************

    if (turnpid_image.out > 0)//左转
    {
        target_left = base_speed  - (int)turnpid_image.out;
        target_right = base_speed + (int)(0.5*turnpid_image.out);
    }
    else//右转
    {
        target_left = base_speed  - (int)(0.5*turnpid_image.out);
        target_right = base_speed + (int)turnpid_image.out;
    }
}
/***********************************************
* @brief : 电磁转向环位置式PD控制
* @param : void
* @return: void
* @date  : 2023.7.2
* @author: L & 刘骏帆
************************************************/
void PIDTurnADC(void)
{
    turnpid_adc.err = ChaBiHe(TRACK);//电磁偏差
    float EC = turnpid_adc.err - turnpid_adc.last_err;//偏差变化量

    turnpid_adc.out = (int)(turnpid_adc.P * turnpid_adc.err + turnpid_adc.D * EC + gyropid.P*real_gyro);//PID公式计算输出量
    turnpid_adc.last_err = turnpid_adc.err;//更新上一次偏差

    if (turnpid_adc.out > 0)//左转
    {
        target_left = base_speed - (int)turnpid_adc.out;
        target_right = base_speed + (int)(0.3*turnpid_adc.out);
    }
    else//右转
    {
        target_left = base_speed - (int)(0.3*turnpid_adc.out);
        target_right = base_speed + (int)turnpid_adc.out;
    }
}
/***********************************************
* @brief : 初始化前馈控制参数
* @param : ffc:前馈控制结构体
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
* @brief : 前馈控制
* @param : target:前馈控制输入
*          ffc:前馈控制结构体
* @return: ffc->output:前馈控制输出量
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
