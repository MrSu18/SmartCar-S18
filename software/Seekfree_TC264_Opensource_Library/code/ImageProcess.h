/*
 * ImageProcess.h
 *
 *  Created on: 2023年3月1日
 *      Author: L
 */

#ifndef CODE_IMAGEPROCESS_H_
#define CODE_IMAGEPROCESS_H_

#include "zf_common_headfile.h"
#include "zf_device_mt9v03x.h"
#include "motor.h"

//============================参数================================
#define TRACK_WIDTH         0.4             //赛道宽度(m)
#define OUT_THRESHOLD       100             //出界判断阈值
#define PROCESS_LENGTH      30              //状态机的长度
//===============================================================

typedef struct ProcessProperty//进入元素之后的一些特殊属性
{
    uint8 max_speed;//元素内提速要提速到多少
    uint8 min_speed;//元素降速要降速到多少
    uint8 integral;//编码器或者陀螺仪积分到多少算一个状态
}ProcessProperty;

extern uint8 process_status[PROCESS_LENGTH];
extern uint16 process_speed[PROCESS_LENGTH];
extern ProcessProperty process_property[PROCESS_LENGTH];
extern uint8 process_status_cnt;//元素状态数组的计数器

void OutProtect(void);
void ImageProcess(void);
void TrackBasicClear(void);//赛道基础信息变量重置，为下一帧做准备
void ProcessPropertyInit(void);//状态机变量属性初始化

#endif /* CODE_IMAGEPROCESS_H_ */
