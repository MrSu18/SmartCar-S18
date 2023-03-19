/*
 * ImageSpecial.h
 *
 *  Created on: 2023��3��14��
 *      Author: L
 */

#ifndef CODE_IMAGESPECIAL_H_
#define CODE_IMAGESPECIAL_H_

#include "zf_common_headfile.h"

typedef enum Cut_Type
{
    CUT_IN = 0,
    CUT_OUT,
}Cut_Type;

extern Cut_Type cut_type;

//���⺯��
uint8 GarageFindCorner(int* corner_id);
uint8 GarageIdentify_L(void);
uint8 GarageIdentify_R(void);
void EdgeDetection_Garage(uint8 flag);
//ʮ��·�ںͶ�·����
uint8 CutIdentify(void);
uint8 CrossFindCorner(int* corner_id_l,int* corner_id_r);
uint8 CrossIdentify(void);
void EdgeDetection_Cross(void);

#endif /* CODE_IMAGESPECIAL_H_ */
