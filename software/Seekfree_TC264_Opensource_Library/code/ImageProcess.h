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
#define TRACK_HALF_WIDTH    22.5            //赛道半宽像素点
#define OUT_THRESHOLD       100             //出界判断阈值
//===============================================================

void OutProtect(void);                      //出界保护程序

#endif /* CODE_IMAGEPROCESS_H_ */
