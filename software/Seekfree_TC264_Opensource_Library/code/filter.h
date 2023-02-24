/*
 * filter.h
 *
 *  Created on: 2023年1月7日
 *      Author: L
 */

#ifndef CODE_FILTER_H_
#define CODE_FILTER_H_

#include "zf_common_headfile.h"

typedef struct
{
    int16 x;                    //预测值
    float A;                    //x(k)=A*x(k-1)+B*u(k),u(k)~N(0,Q)
    float H;                    //z(k)=H*x(k)+w(k),w(k)~N(0,R)
    float Q;                    //预测噪声协方差
    float R;                    //测量误差协方差
    float p;                    //估计误差协方差
    float gain;                 //卡尔曼增益
}kalman_filter_1;

extern kalman_filter_1 kalman_adc;

void KalmanInit(kalman_filter_1* state,float Q,float R);                //初始化卡尔曼滤波的系数
int16 KalmanFilter(kalman_filter_1* state,float z);                     //卡尔曼滤波
int16 SecondOrderFilter_L(int16 value);                                 //左编码器二阶低通滤波
int16 SecondOrderFilter_R(int16 value);                                 //右编码器二阶低通滤波

#endif /* CODE_FILTER_H_ */
