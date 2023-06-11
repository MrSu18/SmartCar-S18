/*
 * FuzzyPID.h
 *
 *  Created on: 2023年4月13日
 *      Author: L
 */

#ifndef CODE_FUZZYPID_H_
#define CODE_FUZZYPID_H_

//论域各个区间的端点
#define NB      -3
#define NM      -2
#define NS      -1
#define ZO      0
#define PS      1
#define PM      2
#define PB      3
//图像模糊PID需要用到的各个参数
#define E_MAX       13
#define E_MIN       -13
#define EC_MAX      2
#define EC_MIN      -2
#define KP_MAX      20  //800
#define KP_MIN      0
#define KD_MAX      6   //60
#define KD_MIN      0
//电磁模糊PID需要用到的各个参数
#define E_MAX_A       20
#define E_MIN_A       -20
#define EC_MAX_A      10
#define EC_MIN_A      -10
#define KP_MAX_A      14
#define KP_MIN_A      0
#define KD_MAX_A      8
#define KD_MIN_A      0

extern float EC;

void Fuzzification(float E, float EC, float memership[4], int index_E[2], int index_EC[2]);     //模糊化
void SoluteFuzzy(float qE, float qEC, int8 rule_KP[7][7], int8 rule_KD[7][7]);                  //模糊推理
void FuzzyPID(void);                                                                            //图像模糊PID计算
float Quantization(float max, float min, float x);                                              //将x映射到[-3，3]区间
float InverseQuantization(float max, float min, float x);                                       //将x反映射
void FuzzyPID_ADC(void);                                                                        //电磁模糊PID计算
void GetTurnImagePID(void);//人为根据偏差设定出转向P和D

#endif /* CODE_FUZZYPID_H_ */
