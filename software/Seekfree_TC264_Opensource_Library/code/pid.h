/*
 * pid.h
 *
 *  Created on: 2023��1��9��
 *      Author: L
 */

#ifndef CODE_PID_H_
#define CODE_PID_H_

#include "zf_common_headfile.h"
#include "motor.h"
#include "adc.h"
#include "key.h"

//PID�����Ľṹ��
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

void PIDInit(PID* pid,float P,float I,float D);                                 //��ʼ��PID����
int PIDSpeedLeft(int16 encoder_data,int16 target_data,PID* pid);                //����PID����
int PIDSpeedRight(int16 encoder_data,int16 target_data,PID* pid);               //����PID����
int PIDTurn(PID* pid);                                                          //ת��PID
void AutoPID(void);                                                             //�����ٶȻ�PID�Զ�������ȡֵ

#endif /* CODE_PID_H_ */
