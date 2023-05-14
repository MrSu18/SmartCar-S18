/*
 * ImageSpecial.h
 *
 *  Created on: 2023年3月14日
 *      Author: L
 */

#ifndef CODE_IMAGESPECIAL_H_
#define CODE_IMAGESPECIAL_H_

#include "zf_common_headfile.h"

//坡道函数
uint8 SlopeIdentify(void);//坡道状态机
float CalculateGradient(uint8 lr_flag);//坡道计算边线斜率
//路障函数
extern int time;
extern uint8 timeintegral_flag;
uint8 BarrierIdentify(void);//路障状态机
//车库函数
uint8 GarageFindCorner(int* corner_id);//车库找角点
uint8 GarageIdentify_L(void);//左车库状态机
uint8 GarageIdentify_R(void);//右车库状态机
//断路函数
uint8 CutIdentify(void);//断路状态机
uint8 CutFindCorner(int16* corner_id_l,int16* coener_id_r);//断路找角点
//十字路口函数
uint8 CrossIdentify(void);//十字状态机
uint8 CrossFindCorner(int16* corner_id_l, int16* corner_id_r);//十字找角点
void EdgeDetection_Cross(uint8 lr_flag);
//左环岛元素函数
void LeftLineDetectionAgain(void);
void RightLineDetectionAgain(void);
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



#endif /* CODE_IMAGESPECIAL_H_ */
