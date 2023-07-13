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
#include "Menu.h"

//PID�����Ľṹ��
typedef struct
{
   float P;
   float I;
   float D;
   float integer_err;
   float err;
   float last_err;
   float errc;//ƫ��仯��
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
//ͼ��ת��PID
extern PID turnpid_image;
extern float turnright_P;//��תת���P
extern float turnright_D;//��תת���D
extern float turngyro_right;//��ת���ٶ����Ƶ�����
extern PID turnpid_adc;
extern PID gyropid;
extern FFC speedffc_left;
extern FFC speedffc_right;
extern float image_bias;
extern int16 real_gyro;
extern int16 last_real_gyro;//��һ�ν��ٶ�

void PIDInit(PID* pid,float P,float I,float D);                          //��ʼ��PID����
int PIDSpeed(int encoder_data,int target_data,PID* pid);                 //�ٶȻ�PID����
void PIDTurnImage(void);                                                //����ͷת��PID
void PIDTurnADC(void);                                                  //���ת��PID
void FFCInit(FFC* ffc,float conf_1,float conf_2,float conf_3);
int FeedForwardCtrl(int target,FFC* ffc);

#endif /* CODE_PID_H_ */
