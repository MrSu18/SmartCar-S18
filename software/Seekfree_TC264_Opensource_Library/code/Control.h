/*
 * Control.h
 *
 *  Created on: 2023��4��17��
 *      Author: 30516
 */

#ifndef CODE_CONTROL_H_
#define CODE_CONTROL_H_

#include "zf_common_typedef.h"

typedef struct ControlParam//��ͬ�ٶȶ�Ӧ�Ĳ�ͬ����
{
    int speed;//�ٶ�
    float turn_kp,turn_kd,turn_gkd;//ת�򻷵�P��D��GD
    float aim;//Ԥ���
}ControlParam;

typedef enum SpeedType
{
    kImageSpeed=0,//ʹ��ͼ���ٶȾ���
    kNormalSpeed,//ȡ���ٶȾ���
}SpeedType;//��Ľṹ��

extern ControlParam contro_param[10];
extern enum SpeedType speed_type;//�����ٶȾ��ߵ�����
extern uint16 original_speed;//�趨���ٶ�
extern int s;//�ٶȾ���ͼ�񿴵���ֱ���ĳ���
extern float speed_detection_a;//�ٶȾ��ߵļ��ٶ�

void ControlParmInit(void);//��ʼ�����Ʋ���
uint16 SpeedDecision(uint16 original_speed,float a);//�ٶȾ���
void OutGarage(void);//д������

#endif /* CODE_CONTROL_H_ */
