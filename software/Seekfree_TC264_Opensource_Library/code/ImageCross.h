/*
 * ImageCross.h
 *
 *  Created on: 2023年3月10日
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
    CUT_IN = 0,//判断是否为断路，是就跳到CUT_IN
    CUT_OUT,//切换为电磁循迹,判断是否为断路出口，是则跳到CUT_IN,回到摄像头循迹
}Cut_Type;

extern Cross_Type cross_type;
extern Cut_Type   cut_type;

uint8 CutIdentify(void);
uint8 CrossFindCorner(int* corner_id_l,int* corner_id_r);
uint8 CrossIdentify(void);
void EdgeDetection_Cross(void);



#endif /* CODE_IMAGECROSS_H_ */
