/*
 * ImageSpecial.h
 *
 *  Created on: 2023年3月14日
 *      Author: L
 */

#ifndef CODE_IMAGESPECIAL_H_
#define CODE_IMAGESPECIAL_H_

#include "zf_common_headfile.h"

//车库函数
uint8 GarageFindCorner(void);
uint8 GarageIdentify_L(void);
uint8 GarageIdentify_R(void);
//十字路口和断路函数
uint8 CutIdentify(void);
uint8 CrossFindCorner(int* corner_id_l,int* corner_id_r);
uint8 CrossIdentify(void);
void EdgeDetection_Cross(void);

#endif /* CODE_IMAGESPECIAL_H_ */
