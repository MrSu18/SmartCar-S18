/*
 * filter.c
 *
 *  Created on: 2023��1��7��
 *      Author: L
 */
#include "filter.h"

kalman_filter_1 kalman_adc;                         //ADC�������˲�ϵ��
int16 last_left = 0,last_2_left = 0;                //����������׵�ͨ�˲��Ĺ�ȥֵ
int16 last_right = 0,last_2_right = 0;              //�ұ��������׵�ͨ�˲��Ĺ�ȥֵ
/***********************************************
* @brief : �������˲���ʼ��
* @param : state:�������˲����ݵĽṹ��ָ��
*          Q:Ԥ������Э����
*          R:�������Э����
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
* @brief : һ�׿������˲�
* @param : state:�������˲����ݵĽṹ��ָ��
*          z:ԭʼ��������
* @return: state->x:�˲���Ĺ���ֵ
* @date  : 2023.1.7
* @author: L
************************************************/
int16 KalmanFilter(kalman_filter_1* state,float z)
{
    //�������ֵ��x(k)=A*x(k-1)+B*u(k)
    state->x = state->A*state->x;
    //�������Э���p(k)=A*p(k-1)*A^T+Q
    state->p = state->A*state->p*state->A+state->Q;

    //���㿨��������:gain=(p(k)*H^T)/(H*p(k)*H^T+R)
    state->gain = (state->p*state->H)/(state->H*state->p*state->H+state->R);
    //�������ֵ��x(k)=x(k-1)+gain*(z-H*x(k-1))
    state->x = state->x+state->gain*(z-state->H*state->x);
    //�������Э����:p(k)=(I-gain*H)*p(k-1)
    state->p = (1-state->gain*state->H)*state->p;

    return state->x;
}
/***********************************************
* @brief : ���׵�ͨ�˲�,�������
* @param : value:��Ҫ�˲���ֵ
* @return: �˲����ֵ
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
* @brief : ���׵�ͨ�˲�,�ұ�����
* @param : value:��Ҫ�˲���ֵ
* @return: �˲����ֵ
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
