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
#include "KeyMenu.h"

//PID参数的结构体
typedef struct
{
   float P;
   float I;
   float D;
   float integer_err;
   float err;
   float last_err;
   int   out;
}PID;

typedef struct
{
   float conf_1;
   float conf_2;
   float conf_3;
   int iuput;
   int last_input;
   int pre_input;
   int output;
}FFC;

extern PID speedpid_left;
extern PID speedpid_right;
extern PID turnpid_image;
extern PID turnpid_adc;
extern PID gyropid;
extern FFC speedffc_left;
extern FFC speedffc_right;
extern float image_bias;
extern int16 real_gyro;
extern uint8 gyro_flag;

void PIDInit(PID* pid,float P,float I,float D);                                     //初始化PID参数
int PIDSpeed(int encoder_data,int target_data,PID* pid);                            //速度环PID计算
void PIDTurnImage(int* target_left,int* target_right,PID* pid);                     //摄像头转向环PID
void PIDTurnADC(int* target_left,int* target_right,PID* pid);                       //电磁转向环PID
void PIDClear(void);                                                                //复位PID的参数
void FFCInit(FFC* ffc,float conf_1,float conf_2,float conf_3);
int FeedForwardCtrl(int target,FFC* ffc);

#endif /* CODE_PID_H_ */
