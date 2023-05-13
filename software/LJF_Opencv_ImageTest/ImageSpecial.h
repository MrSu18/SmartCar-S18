//
// Created by 30516 on 2023/3/10.
//

#ifndef LJF_OPENCV_IMAGETEST_IMAGESPECIAL_H
#define LJF_OPENCV_IMAGETEST_IMAGESPECIAL_H

#include "main.h"

void LeftLineDetectionAgain(void);
void RightLineDetectionAgain(void);
//左环岛元素函数
uint8 CircleIslandLStatus(void);//左环岛状态状态机
uint8 CircleIslandLDetection(void);//检测左环岛
uint8 CircleIslandLInDetection(void);//检测是否到了需要入环的状态
uint8 CircleIslandLIn(void);//左环岛入环处理
uint8 CircleIslandLOutDetection(void);//环岛检测出环
uint8 CircleIslandLOutFinish(void);//检测环岛是否结束
void CircleIslandLOut(void);//左环岛出环
uint8 CircleIslandLEnd(void);//判断左环岛是否结束
//右环岛元素函数
uint8 CircleIslandRStatus(void);//右环岛状态机
uint8 CircleIslandRDetection(void);//检测右环岛
uint8 CircleIslandRInDetection(void);//检测是否到了需要入环的状态
uint8 CircleIslandRIn(void);//右环岛入环处理
uint8 CircleIslandROutDetection(void);//环岛检测出环
uint8 CircleIslandROutFinish(void);//检测环岛是否结束
void CircleIslandROut(void);//右环岛出环
uint8 CircleIslandREnd(void);//判断右环岛是否结束


#endif //LJF_OPENCV_IMAGETEST_IMAGESPECIAL_H
