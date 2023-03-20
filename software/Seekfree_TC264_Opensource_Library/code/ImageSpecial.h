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
uint8 GarageFindCorner(int* corner_id);
uint8 GarageIdentify_L(void);
uint8 GarageIdentify_R(void);
void EdgeDetection_Garage(uint8 flag);
//ʮ��·�ںͶ�·����
uint8 CutIdentify(void);
uint8 CrossFindCorner(int* corner_id_l,int* corner_id_r);
uint8 CrossIdentify(void);
void EdgeDetection_Cross(void);
//�󻷵�Ԫ�غ���
void LeftLineDetectionAgain(void);
void RightLineDetectionAgain(void);
uint8 CircleIslandLStatus(void);//�󻷵�״̬״̬��
uint8 CircleIslandLDetection(void);//����󻷵�
uint8 CircleIslandLInDetection(void);//����Ƿ�����Ҫ�뻷��״̬
uint8 CircleIslandLIn(void);//�󻷵��뻷����
uint8 CircleIslandLOutDetection(void);//����������
uint8 CircleIslandLOut(void);//�󻷵�����
uint8 CircleIslandLEnd(void);//�ж��󻷵��Ƿ����


#endif /* CODE_IMAGESPECIAL_H_ */
