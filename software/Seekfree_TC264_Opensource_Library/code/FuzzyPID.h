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
#define E_MAX       15  //使用舵轮转速纯跟踪算法时20
#define E_MIN       -15 //-20
#define EC_MAX      10  //40
#define EC_MIN      -10 //-40
#define KP_MAX      22  //25
#define KP_MIN      0
#define KD_MAX      6   //8
#define KD_MIN      0
//电磁模糊PID需要用到的各个参数
#define E_MAX_A       20
#define E_MIN_A       -20
#define EC_MAX_A      15
#define EC_MIN_A      -15
#define KP_MAX_A      5
#define KP_MIN_A      0
#define KD_MAX_A      3
#define KD_MIN_A      0

void Fuzzification(float E, float EC, float memership[4], int index_E[2], int index_EC[2]);     //模糊化
void SoluteFuzzy(float qE, float qEC);                                                          //模糊推理
void FuzzyPID(void);                                                                            //图像模糊PID计算
float Quantization(float max, float min, float x);                                              //将x映射到[-3，3]区间
float InverseQuantization(float max, float min, float x);                                       //将x反映射
void FuzzyPID_ADC(void);                                                                        //电磁模糊PID计算

#endif /* CODE_FUZZYPID_H_ */
