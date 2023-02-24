/*
 * pid.c
 *
 *  Created on: 2023年1月9日
 *      Author: L
 */
#include "pid.h"

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
* @brief : 左边电机速度环增量式PI控制
* @param : encoder_data:左编码器的值
*          target_data:左边的目标值
*          pid:速左边电机度环的PID
* @return: out:给左电机的PWM大小
* @date  : 2023.1.17
* @author: L
************************************************/
int PIDSpeedLeft(int16 encoder_speed,int16 target_speed,PID* pid)
{
    pid->err = target_speed-encoder_speed;                                  //计算偏差

    pid->out += (int)(pid->P*(pid->err-pid->last_err)+pid->I*pid->err);
    pid->last_err = pid->err;                                               //保存上一次的值

    return pid->out;
}
/***********************************************
* @brief : 右边电机速度环增量式PI控制
* @param : encoder_data:右编码器的值
*          target_data:右边的目标值
*          pid:右边电机速度环的PID
* @return: out:给右电机的PWM大小
* @date  : 2023.1.17
* @author: L
************************************************/
int PIDSpeedRight(int16 encoder_speed,int16 target_speed,PID* pid)
{
    pid->err = target_speed-encoder_speed;                                      //计算偏差

    pid->out += (int)(pid->P*(pid->err-pid->last_err)+pid->I*pid->err);
    pid->last_err = pid->err;                                                   //保存上一次的值

    return pid->out;
}
/***********************************************
* @brief : 转向环位置式PD控制
* @param : pid:转向环的PID
* @return: out:左右轮目标速度的变化量
* @date  : 2023.2.2
* @author: L
************************************************/
int PIDTurn(PID* pid)
{
    //获取此时偏差
    ADCGetValue(adc_value);
    ChaBiHe(&pid->err,TRACK);

    pid->out = pid->P*pid->err+pid->D*(pid->err-pid->last_err);
    pid->last_err = pid->err;                                               //保存上一次的值

    return pid->out;
}
/***********************************************
* @brief : 开环获取两个编码器加速到稳定的值，并以一定时序发送到串口
* @param : void
* @return: void
* @date  : 2023.1.30
* @author: L
************************************************/
void AutoPID(void)
{
    int temp=0;
    int16 data1=0,data2=0;

    while(1){
    if(KEY1){
        system_delay_ms(1000);
        encoder_clear_count(ENCODER_LEFT);
        encoder_clear_count(ENCODER_RIGHT);
        MotorSetPWM(1500,1500);
        while(1)
        {
            EncoderGetCount(&data1,&data2);
            printf("%d      ",data1);
            printf("%d\n",data2);
            if(temp>=300)
                MotorSetPWM(0,0);
            temp++;
            system_delay_ms(10);
        }
    }
    }
}
