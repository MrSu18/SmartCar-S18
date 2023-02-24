/*
 * filter.c
 *
 *  Created on: 2023年1月7日
 *      Author: L
 */
#include "filter.h"

kalman_filter_1 kalman_adc;                         //ADC卡尔曼滤波系数
int16 last_left = 0,last_2_left = 0;                //左编码器二阶低通滤波的过去值
int16 last_right = 0,last_2_right = 0;              //右编码器二阶低通滤波的过去值
/***********************************************
* @brief : 卡尔曼滤波初始化
* @param : state:卡尔曼滤波数据的结构体指针
*          Q:预测噪声协方差
*          R:测量误差协方差
* @return: void
* @date  : 2023.1.7
* @author: L
************************************************/
void KalmanInit(kalman_filter_1* state,float Q,float R)
{
    state->x = 0;
    state->A = 1.0f;
    state->H = 1.0f;
    state->Q = Q;
    state->R = R;
    state->p = 0.0f;
}
/***********************************************
* @brief : 一阶卡尔曼滤波
* @param : state:卡尔曼滤波数据的结构体指针
*          z:原始测量数据
* @return: state->x:滤波后的估计值
* @date  : 2023.1.7
* @author: L
************************************************/
int16 KalmanFilter(kalman_filter_1* state,float z)
{
    //先验估计值：x(k)=A*x(k-1)+B*u(k)
    state->x = state->A*state->x;
    //先验误差协方差：p(k)=A*p(k-1)*A^T+Q
    state->p = state->A*state->p*state->A+state->Q;

    //计算卡尔曼增益:gain=(p(k)*H^T)/(H*p(k)*H^T+R)
    state->gain = (state->p*state->H)/(state->H*state->p*state->H+state->R);
    //后验估计值：x(k)=x(k-1)+gain*(z-H*x(k-1))
    state->x = state->x+state->gain*(z-state->H*state->x);
    //后验误差协方差:p(k)=(I-gain*H)*p(k-1)
    state->p = (1-state->gain*state->H)*state->p;

    return state->x;
}
/***********************************************
* @brief : 二阶低通滤波,左编码器
* @param : value:需要滤波的值
* @return: 滤波后的值
* @date  : 2023.1.19
* @author: L
************************************************/
int16 SecondOrderFilter_L(int16 value)
{
    value = 0.2*value+0.4*last_left+0.4*last_2_left;
    last_2_left = last_left;
    last_left = value;

    return value;
}
/***********************************************
* @brief : 二阶低通滤波,右编码器
* @param : value:需要滤波的值
* @return: 滤波后的值
* @date  : 2023.1.19
* @author: L
************************************************/
int16 SecondOrderFilter_R(int16 value)
{
    value = 0.2*value+0.4*last_right+0.4*last_2_right;
    last_2_right = last_right;
    last_right = value;

    return value;
}
