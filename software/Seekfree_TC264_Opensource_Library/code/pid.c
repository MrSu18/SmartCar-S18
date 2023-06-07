/*
 * pid.c
 *
 *  Created on: 2023年1月9日
 *      Author: L
 */
#include "pid.h"
#include "ImageTrack.h"

PID speedpid_left;                          //赛道左轮速度环PID
PID speedpid_right;                         //赛道右轮速度环PID
PID turnpid_image;                          //图像转向环PID
PID turnpid_adc;                            //电磁转向环PID
FFC speedffc_left;
FFC speedffc_right;
PID gyropid;
int16 real_gyro = 0;
uint8 gyro_flag = 0;

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
int PIDSpeed(int encoder_speed,int target_speed,PID* pid)
{
    pid->err = (float)(target_speed-encoder_speed);                                  //计算偏差

    pid->out += (int)(pid->P*(pid->err-pid->last_err)+pid->I*pid->err);
    pid->last_err = pid->err;                                                        //保存上一次的值

    return pid->out;
}
/***********************************************
* @brief : 摄像头转向环位置式PD控制
* @param : pid:图像转向环的PID
* @return: out:左右轮目标速度的变化量
* @date  : 2023.3.2
* @author: L
************************************************/
void PIDTurnImage(int* target_left,int* target_right,PID* pid)
{
    pid->err = image_bias;

    pid->out = (int)(pid->P*pid->err+pid->D*(pid->err-pid->last_err));
    pid->last_err = pid->err;                                               //保存上一次的值

#if 0
    //内减外加差速    转向PID P:13 D:0 速度:120 预瞄点:0.32 连续弯道会加速，暂时不采用这种
    *target_left = base_speed - pid->out;
    *target_right = base_speed + pid->out;
#else
    //内轮减速外轮不变
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
#endif
}
/***********************************************
* @brief : 电磁转向环位置式PD控制
* @param : pid:电磁转向环的PID
* @return: out:左右轮目标速度的变化量
* @date  : 2023.3.16
* @author: L
************************************************/
void PIDTurnADC(int* target_left,int* target_right,PID* pid)
{
        //获取此时电磁偏差
        ADCGetValue(adc_value);
//        ChaBiHe(&pid->err,TRACK);

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
/***********************************************
* @brief : 对摄像头PID的相关参数复位
* @param : void
* @return: void
* @date  : 2023.6.7
* @author: L
************************************************/
void PIDClear(void)
{
    PIDInit(&speedpid_left,185.8,0.61,0);
    PIDInit(&speedpid_right,164.8,0.54,0);
    PIDInit(&turnpid_image,13,0,2);
    PIDInit(&turnpid_adc, 0, 0, 0);
}

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
