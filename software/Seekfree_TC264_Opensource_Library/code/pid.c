/*
 * pid.c
 *
 *  Created on: 2023年1月9日
 *      Author: L
 */
#include "pid.h"
#include "ImageTrack.h"

PID speedpid_left;                          //左轮速度环PID
PID speedpid_right;                         //右轮速度环PID
PID turnpid;                                //转向环PID

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
* @date  : 2023.1.17
* @author: L
************************************************/
int PIDSpeed(int16 encoder_speed,int16 target_speed,PID* pid)
{
    pid->err = target_speed-encoder_speed;                                  //计算偏差

    pid->out += (int)(pid->P*(pid->err-pid->last_err)+pid->I*pid->err);
    pid->last_err = pid->err;                                               //保存上一次的值

    return pid->out;
}
/***********************************************
* @brief : 转向环位置式PD控制
* @param : pid:转向环的PID
* @return: out:左右轮目标速度的变化量
* @date  : 2023.3.2
* @author: L
************************************************/
void PIDTurn(int16* target_left,int16* target_right,PID* pid)
{
//    //获取此时偏差
//    ADCGetValue(adc_value);
//    ChaBiHe(&pid->err,TRACK);
    pid->err = image_bias;

    pid->out = (int)(pid->P*pid->err+pid->D*(pid->err-pid->last_err));
    pid->last_err = pid->err;                                               //保存上一次的值

    if(pid->out>0)//左转
    {
        *target_left = base_speed - pid->out;
        *target_right = base_speed;
    }
    else
    {
        *target_left = base_speed;
        *target_right = base_speed + pid->out;
    }
}
