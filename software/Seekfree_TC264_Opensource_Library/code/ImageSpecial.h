/*
 * ImageSpecial.h
 *
 *  Created on: 2023年3月14日
 *      Author: L
 */

#ifndef CODE_IMAGESPECIAL_H_
#define CODE_IMAGESPECIAL_H_

#include "zf_common_headfile.h"

//#define TRUE        1
//#define FALSE       0

//车库函数
uint8 GarageFindCorner(int* corner_id);
uint8 GarageIdentify_L(void);
uint8 GarageIdentify_R(void);
void EdgeDetection_Garage(uint8 flag);
//断路函数
uint8 CutIdentify(void);
uint8 CutFindCorner(int16* corner_id_l,int16* coener_id_r);
//十字路口函数
uint8 CrossIdentify(void);
uint8 CrossFindCorner(int16* corner_id_l, int16* corner_id_r);
void EdgeDetection_Cross(void);
//左环岛元素函数
void LeftLineDetectionAgain(void);
void RightLineDetectionAgain(void);
uint8 CircleIslandLStatus(void);//左环岛状态状态机
uint8 CircleIslandLDetection(void);//检测左环岛
uint8 CircleIslandLInDetection(void);//检测是否到了需要入环的状态
uint8 CircleIslandLIn(void);//左环岛入环处理
uint8 CircleIslandLOutDetection(void);//环岛检测出环
uint8 CircleIslandLOut(void);//左环岛出环
uint8 CircleIslandLEnd(void);//判断左环岛是否结束


#endif /* CODE_IMAGESPECIAL_H_ */
