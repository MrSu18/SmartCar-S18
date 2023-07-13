/*
 * Control.h
 *
 *  Created on: 2023年4月17日
 *      Author: 30516
 */

#ifndef CODE_CONTROL_H_
#define CODE_CONTROL_H_

#include "zf_common_typedef.h"

typedef struct ControlParam//不同速度对应的不同参数
{
    int speed;//速度
    float turn_kp,turn_kd,turn_gkd;//转向环的P、D、GD
    float aim;//预瞄点
}ControlParam;

typedef enum SpeedType
{
    kImageSpeed=0,//使用图像速度决策
    kNormalSpeed,//取消速度决策
}SpeedType;//点的结构体

extern ControlParam contro_param[10];
extern enum SpeedType speed_type;//基础速度决策的依据
extern uint16 original_speed;//设定的速度
extern int s;//速度决策图像看到长直道的长度
extern float speed_detection_a;//速度决策的加速度

void ControlParmInit(void);//初始化控制参数
uint16 SpeedDecision(uint16 original_speed,float a);//速度决策
void OutGarage(void);//写死出库

#endif /* CODE_CONTROL_H_ */
