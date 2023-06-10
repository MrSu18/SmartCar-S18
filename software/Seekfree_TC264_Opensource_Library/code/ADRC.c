

#include "ADRC.h"
Fhan_Data adrc_controller_gyro_out;//角速度滤波
Fhan_Data adrc_controller_err;//用于模糊PID偏差滤波
Fhan_Data adrc_controller_errc;//用于模糊PID偏差变化量滤波
/***********************************************
* @brief : 开根号
* @param : float number 输入量
* @return: float
* @date  : 2023.5.25
* @author: 黄诚钜
************************************************/

float my_sqrt(float number)
{
    long i;
    float x, y;
    const float f = 1.5F;
    x = number * 0.5F;
    y = number;
    i = * ( long * ) &y;
    i = 0x5f3759df - ( i >> 1 );

    y = * ( float * ) &i;
    y = y * ( f - ( x * y * y ) );
    y = y * ( f - ( x * y * y ) );
    return number * y;
}
/***********************************************
* @brief : 取符号函数
* @param : float Input 输入量
* @return: 符号函数
* @date  : 2023.5.25
* @author: 黄诚钜
************************************************/

int16_t Sign_ADRC(float Input)
{
    int16_t output=0;
    if(Input>1E-6) output=1;
    else if(Input<-1E-6) output=-1;
    else output=0;
    return output;
}
/***********************************************
* @brief : ADRC参数初始化
* @param :
* @return: viod
* @date  : 2023.5.25
* @author: 黄诚钜
************************************************/
int16_t Fsg_ADRC(float x,float d)
{
    int16_t output=0;
    output=(Sign_ADRC(x+d)-Sign_ADRC(x-d))/2;
    return output;
}
/***********************************************
* @brief : ADRC参数初始化
* @param :
* @return: viod
* @date  : 2023.5.25
* @author: 黄诚钜
************************************************/
void ADRC_Init(void)
{
    adrc_controller_gyro_out.r=1000000;
    adrc_controller_gyro_out.h=0.004;
    adrc_controller_gyro_out.N0=2;

    adrc_controller_errc.r=1000000;
    adrc_controller_errc.h=0.008;
    adrc_controller_errc.N0=2;
}

/***********************************************
* @brief : ADRC最速跟踪微分器TD，改进的算法fhan
* @param : Fhan_Data *fhan_Input:输入fhan 参数
*          float expect_ADRC : 期望
* @return: 微分跟踪
* @date  : 2023.5.25
* @author: 黄诚钜
* @用例    Fhan_ADRC(&ADRC_Controller,Speed.output);//对输出量进行TD 响应速度减慢，但是收敛时间减小
*          ADRC_Controller->x1 为最新跟踪状态量 可作为 Speed.output 的替代输出量
************************************************/
void Fhan_ADRC(Fhan_Data *fhan_Input,float expect_ADRC)//安排ADRC过度过程
{
    float d=0,a0=0,y=0,a1=0,a2=0,a=0;
    float x1_delta=0;//ADRC状态跟踪误差项
    x1_delta=fhan_Input->x1-expect_ADRC;//用x1-v(k)替代x1得到离散更新公式
    fhan_Input->h0=fhan_Input->N0*fhan_Input->h;//用h0替代h，解决最速跟踪微分器速度超调问题
    d=fhan_Input->r*fhan_Input->h0*fhan_Input->h0;//d=rh^2;
    a0=fhan_Input->h0*fhan_Input->x2;//a0=h*x2
    y=x1_delta+a0;//y=x1+a0
    a1=my_sqrt(d*(d+8*ABS(y)));//a1=sqrt(d*(d+8*ABS(y))])
    a2=a0+Sign_ADRC(y)*(a1-d)/2;//a2=a0+sign(y)*(a1-d)/2;
    a=(a0+y)*Fsg_ADRC(y,d)+a2*(1-Fsg_ADRC(y,d));
    fhan_Input->fh=-fhan_Input->r*(a/d)*Fsg_ADRC(a,d)
                   -fhan_Input->r*Sign_ADRC(a)*(1-Fsg_ADRC(a,d));//得到最速微分加速度跟踪量
    fhan_Input->x1+=fhan_Input->h*fhan_Input->x2;//跟新最速跟踪状态量x1
    fhan_Input->x2+=fhan_Input->h*fhan_Input->fh;//跟新最速跟踪状态量微分x2
}
