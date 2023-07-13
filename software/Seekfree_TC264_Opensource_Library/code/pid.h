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
#include "Menu.h"

//PID参数的结构体
typedef struct
{
   float P;
   float I;
   float D;
   float integer_err;
   float err;
   float last_err;
   float errc;//偏差变化量
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
//图像转向环PID
extern PID turnpid_image;
extern float turnright_P;//右转转向的P
extern float turnright_D;//右转转向的D
extern float turngyro_right;//右转角速度抑制的增益
extern PID turnpid_adc;
extern PID gyropid;
extern FFC speedffc_left;
extern FFC speedffc_right;
extern float image_bias;
extern int16 real_gyro;
extern int16 last_real_gyro;//上一次角速度

void PIDInit(PID* pid,float P,float I,float D);                          //初始化PID参数
int PIDSpeed(int encoder_data,int target_data,PID* pid);                 //速度环PID计算
void PIDTurnImage(void);                                                //摄像头转向环PID
void PIDTurnADC(void);                                                  //电磁转向环PID
void FFCInit(FFC* ffc,float conf_1,float conf_2,float conf_3);
int FeedForwardCtrl(int target,FFC* ffc);

#endif /* CODE_PID_H_ */
