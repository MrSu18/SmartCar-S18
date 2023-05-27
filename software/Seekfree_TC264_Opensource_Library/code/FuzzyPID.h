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
#define E_MAX       15  //ʹ�ö���ת�ٴ������㷨ʱ20
#define E_MIN       -15 //-20
#define EC_MAX      10  //40
#define EC_MIN      -10 //-40
#define KP_MAX      22  //25
#define KP_MIN      0
#define KD_MAX      6   //8
#define KD_MIN      0
//���ģ��PID��Ҫ�õ��ĸ�������
#define E_MAX_A       20
#define E_MIN_A       -20
#define EC_MAX_A      15
#define EC_MIN_A      -15
#define KP_MAX_A      5
#define KP_MIN_A      0
#define KD_MAX_A      3
#define KD_MIN_A      0

void Fuzzification(float E, float EC, float memership[4], int index_E[2], int index_EC[2]);     //ģ����
void SoluteFuzzy(float qE, float qEC);                                                          //ģ������
void FuzzyPID(void);                                                                            //ͼ��ģ��PID����
float Quantization(float max, float min, float x);                                              //��xӳ�䵽[-3��3]����
float InverseQuantization(float max, float min, float x);                                       //��x��ӳ��
void FuzzyPID_ADC(void);                                                                        //���ģ��PID����

#endif /* CODE_FUZZYPID_H_ */
