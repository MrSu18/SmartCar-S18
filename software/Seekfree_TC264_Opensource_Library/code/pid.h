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
#include "KeyMenu.h"

//PID�����Ľṹ��
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

extern PID speedpid_left;
extern PID speedpid_right;
extern PID turnpid_image;
extern PID turnpid_adc;
extern PID gyropid;
extern float image_bias;
extern int16 real_gyro;
extern uint8 gyro_flag;

void PIDInit(PID* pid,float P,float I,float D);                                     //��ʼ��PID����
int PIDSpeed(int encoder_data,int target_data,PID* pid);                            //�ٶȻ�PID����
void PIDTurnImage(int* target_left,int* target_right,PID* pid);                     //����ͷת��PID
void PIDTurnADC(int* target_left,int* target_right,PID* pid);                       //���ת��PID
void PIDClear(void);                                                                //��λPID�Ĳ���

#endif /* CODE_PID_H_ */
