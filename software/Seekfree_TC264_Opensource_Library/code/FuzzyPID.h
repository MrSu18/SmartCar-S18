/*
 * FuzzyPID.h
 *
 *  Created on: 2023��4��13��
 *      Author: L
 */

#ifndef CODE_FUZZYPID_H_
#define CODE_FUZZYPID_H_

//�����������Ķ˵�
#define NB      -3
#define NM      -2
#define NS      -1
#define ZO      0
#define PS      1
#define PM      2
#define PB      3
//ͼ��ģ��PID��Ҫ�õ��ĸ�������
#define E_MAX       13
#define E_MIN       -13
#define EC_MAX      2
#define EC_MIN      -2
#define KP_MAX      20  //800
#define KP_MIN      0
#define KD_MAX      6   //60
#define KD_MIN      0
//���ģ��PID��Ҫ�õ��ĸ�������
#define E_MAX_A       20
#define E_MIN_A       -20
#define EC_MAX_A      10
#define EC_MIN_A      -10
#define KP_MAX_A      14
#define KP_MIN_A      0
#define KD_MAX_A      8
#define KD_MIN_A      0

extern float EC;

void Fuzzification(float E, float EC, float memership[4], int index_E[2], int index_EC[2]);     //ģ����
void SoluteFuzzy(float qE, float qEC, int8 rule_KP[7][7], int8 rule_KD[7][7]);                  //ģ������
void FuzzyPID(void);                                                                            //ͼ��ģ��PID����
float Quantization(float max, float min, float x);                                              //��xӳ�䵽[-3��3]����
float InverseQuantization(float max, float min, float x);                                       //��x��ӳ��
void FuzzyPID_ADC(void);                                                                        //���ģ��PID����
void GetTurnImagePID(void);//��Ϊ����ƫ���趨��ת��P��D

#endif /* CODE_FUZZYPID_H_ */
