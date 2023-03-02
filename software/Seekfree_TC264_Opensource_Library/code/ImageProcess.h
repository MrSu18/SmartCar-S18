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

#define OUT_THRESHOLD    255                //出界判断阈值

void OutProtect(void);                      //出界保护程序

#endif /* CODE_IMAGEPROCESS_H_ */
