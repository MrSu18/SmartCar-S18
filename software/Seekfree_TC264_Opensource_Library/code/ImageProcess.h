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
//===============================================================

void ImageProcess(void);
void TrackBasicClear(void);//赛道基础信息变量重置，为下一帧做准备

#endif /* CODE_IMAGEPROCESS_H_ */
