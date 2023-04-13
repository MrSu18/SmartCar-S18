/*
 * FuzzyPID.h
 *
 *  Created on: 2023Äê4ÔÂ13ÈÕ
 *      Author: L
 */

#ifndef CODE_FUZZYPID_H_
#define CODE_FUZZYPID_H_

#define NB      -3
#define NM      -2
#define NS      -1
#define ZO      0
#define PS      1
#define PM      2
#define PB      3

#define E_MAX   20
#define E_MIN   -28
#define EC_MAX  40
#define EC_MIN  -40
#define KP_MAX  40
#define KP_MIN  0
#define KD_MAX  10
#define KD_MIN  0

void Fuzzification(float E, float EC, float memership[4], int index_E[2], int index_EC[2]);
void SoluteFuzzy(float qE, float qEC);
void FuzzyPID(void);
float Quantization(float max, float min, float x);
float InverseQuantization(float max, float min, float x);

#endif /* CODE_FUZZYPID_H_ */
