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
extern float Bias;

void PIDInit(PID* pid,float P,float I,float D);                                 //初始化PID参数
int PIDSpeed(int16 encoder_data,int16 target_data,PID* pid);                    //速度环PID计算
void PIDTurn(int16* target_left,int16* target_right,PID* pid);                  //转向环PID

#endif /* CODE_PID_H_ */
