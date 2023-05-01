/*
 * Control.h
 *
 *  Created on: 2023年4月17日
 *      Author: 30516
 */

#ifndef CODE_CONTROL_H_
#define CODE_CONTROL_H_

#include "zf_common_typedef.h"

typedef enum SpeedType
{
    kImageSpeed=0,//使用图像速度决策
    kNormalSpeed,//取消速度决策
}SpeedType;//点的结构体

extern enum SpeedType speed_type;//基础速度决策的依据
extern uint16 original_speed;//设定的速度

uint16 SpeedDecision(uint16 original_speed,float a);//速度决策
void OutGarage(void);//写死出库

#endif /* CODE_CONTROL_H_ */
