//
// Created by Mao Junjie on 2022/6/7.
//

#ifndef ADRC_H
#define ADRC_H
#include "zf_common_headfile.h"
//ADRC跟踪微分器
typedef struct
{
    /*****安排过度过程*******/
    float x1;//跟踪微分期状态量
    float x2;//跟踪微分期状态量微分项
    float r;//时间尺度
    float h;//ADRC系统积分时间
    unsigned short N0;//跟踪微分器解决速度超调h0=N*h

    float h0;
    float fh;//最速微分加速度跟踪量
}Fhan_Data;

extern Fhan_Data adrc_controller_l;
extern Fhan_Data adrc_controller_r;

void ADRC_Init();
void Fhan_ADRC(Fhan_Data *fhan_Input,float expect_ADRC);

//数学运算
float my_sqrt(float number);
#define my_pow(a) ((a)*(a))
#define ABS(x) ( (x)>0?(x):-(x) )
#endif //MM32F3277_ADRC_H
