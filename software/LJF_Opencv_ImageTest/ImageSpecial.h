//
// Created by 30516 on 2023/3/10.
//

#ifndef LJF_OPENCV_IMAGETEST_IMAGESPECIAL_H
#define LJF_OPENCV_IMAGETEST_IMAGESPECIAL_H

#include "main.h"

void LeftLineDetectionAgain(void);
//环岛元素函数
uint8 CircleIslandLStatus(void);//左环岛状态状态机
uint8 CircleIslandLDetection(void);//检测左环岛
uint8 CircleIslandLIn(void );//检测是否到了需要入环的状态

#endif //LJF_OPENCV_IMAGETEST_IMAGESPECIAL_H
