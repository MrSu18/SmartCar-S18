/*
 * ImageSpecial.h
 *
 *  Created on: 2023��3��14��
 *      Author: L
 */

#ifndef CODE_IMAGESPECIAL_H_
#define CODE_IMAGESPECIAL_H_

#include "zf_common_headfile.h"

//�µ�����
uint8 SlopeIdentify(void);//�µ�״̬��
float CalculateGradient(uint8 lr_flag);//�µ��������б��
//·�Ϻ���
extern int time;
extern uint8 timeintegral_flag;
uint8 BarrierIdentify(void);//·��״̬��
//���⺯��
uint8 GarageFindCorner(int* corner_id);//�����ҽǵ�
uint8 GarageIdentify_L(void);//�󳵿�״̬��
uint8 GarageIdentify_R(void);//�ҳ���״̬��
//��·����
uint8 CutIdentify(void);//��·״̬��
uint8 CutFindCorner(int16* corner_id_l,int16* coener_id_r);//��·�ҽǵ�
//ʮ��·�ں���
uint8 CrossIdentify(void);//ʮ��״̬��
uint8 CrossFindCorner(int16* corner_id_l, int16* corner_id_r);//ʮ���ҽǵ�
void EdgeDetection_Cross(uint8 lr_flag);
//�󻷵�Ԫ�غ���
void LeftLineDetectionAgain(void);
void RightLineDetectionAgain(void);
uint8 CircleIslandLStatus(void);//�󻷵�״̬״̬��
uint8 CircleIslandLDetection(void);//����󻷵�
uint8 CircleIslandLInDetection(void);//����Ƿ�����Ҫ�뻷��״̬
uint8 CircleIslandLIn(void);//�󻷵��뻷����
uint8 CircleIslandLOutDetection(void);//����������
uint8 CircleIslandLOutFinish(void);//��⻷���Ƿ����
void CircleIslandLOut(void);//�󻷵�����
uint8 CircleIslandLEnd(void);//�ж��󻷵��Ƿ����
//�һ���Ԫ�غ���
uint8 CircleIslandRStatus(void);//�һ���״̬��
uint8 CircleIslandRDetection(void);//����һ���
uint8 CircleIslandRInDetection(void);//����Ƿ�����Ҫ�뻷��״̬
uint8 CircleIslandRIn(void);//�һ����뻷����
uint8 CircleIslandROutDetection(void);//����������
uint8 CircleIslandROutFinish(void);//��⻷���Ƿ����
void CircleIslandROut(void);//�һ�������
uint8 CircleIslandREnd(void);//�ж��һ����Ƿ����



#endif /* CODE_IMAGESPECIAL_H_ */
