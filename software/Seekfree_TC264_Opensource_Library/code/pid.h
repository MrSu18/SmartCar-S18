/*
 * pid.h
 *
 *  Created on: 2023年1月9日
 *      Author: L
 */

#ifndef CODE_PID_H_
#define CODE_PID_H_

#include "zf_common_headfile.h"
#include "motor.h"
#include "adc.h"
#include "key.h"

//PID参数的结构体
typedef struct
{
   float P;
   float I;
   float D;
   float err;
   float last_err;
   int   out;
}PID;

extern PID speedpid_left;
extern PID speedpid_right;
extern PID turnpid;

void PIDInit(PID* pid,float P,float I,float D);                                 //初始化PID参数
int PIDSpeedLeft(int16 encoder_data,int16 target_data,PID* pid);                //左轮PID计算
int PIDSpeedRight(int16 encoder_data,int16 target_data,PID* pid);               //右轮PID计算
int PIDTurn(PID* pid);                                                          //转向环PID
void AutoPID(void);                                                             //用于速度环PID自动调整获取值

#endif /* CODE_PID_H_ */
