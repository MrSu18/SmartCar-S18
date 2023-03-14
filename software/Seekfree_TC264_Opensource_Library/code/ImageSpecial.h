/*
 * ImageSpecial.h
 *
 *  Created on: 2023��3��14��
 *      Author: L
 */

#ifndef CODE_IMAGESPECIAL_H_
#define CODE_IMAGESPECIAL_H_

#include "zf_common_headfile.h"

//���⺯��
uint8 GarageFindCorner(void);
uint8 GarageIdentify_L(void);
uint8 GarageIdentify_R(void);
//ʮ��·�ںͶ�·����
uint8 CutIdentify(void);
uint8 CrossFindCorner(int* corner_id_l,int* corner_id_r);
uint8 CrossIdentify(void);
void EdgeDetection_Cross(void);

#endif /* CODE_IMAGESPECIAL_H_ */
