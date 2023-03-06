/*
 * ImageProcess.h
 *
 *  Created on: 2023��3��1��
 *      Author: L
 */

#ifndef CODE_IMAGEPROCESS_H_
#define CODE_IMAGEPROCESS_H_

#include "zf_common_headfile.h"
#include "zf_device_mt9v03x.h"
#include "motor.h"

//============================����================================
#define TRACK_WIDTH         0.4             //�������(m)
#define OUT_THRESHOLD       100             //�����ж���ֵ
//===============================================================

void ImageProcess(void);
void TrackBasicClear(void);//����������Ϣ�������ã�Ϊ��һ֡��׼��

#endif /* CODE_IMAGEPROCESS_H_ */
