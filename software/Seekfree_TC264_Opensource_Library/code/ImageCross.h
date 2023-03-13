/*
 * ImageCross.h
 *
 *  Created on: 2023��3��10��
 *      Author: L
 */

#ifndef CODE_IMAGECROSS_H_
#define CODE_IMAGECROSS_H_

#include "zf_common_headfile.h"

typedef enum Cross_Type
{
    CROSS_IN = 0,
    CROSS_OUT,
}Cross_Type;

typedef enum Cut_Type
{
    CUT_IN = 0,//�ж��Ƿ�Ϊ��·���Ǿ�����CUT_IN
    CUT_OUT,//�л�Ϊ���ѭ��,�ж��Ƿ�Ϊ��·���ڣ���������CUT_IN,�ص�����ͷѭ��
}Cut_Type;

extern Cross_Type cross_type;
extern Cut_Type   cut_type;

uint8 CutIdentify(void);
uint8 CrossFindCorner(int* corner_id_l,int* corner_id_r);
uint8 CrossIdentify(void);
void EdgeDetection_Cross(void);



#endif /* CODE_IMAGECROSS_H_ */
