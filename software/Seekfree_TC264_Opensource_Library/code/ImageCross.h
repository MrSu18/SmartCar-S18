/*
 * ImageCross.h
 *
 *  Created on: 2023Äê3ÔÂ10ÈÕ
 *      Author: L
 */

#ifndef CODE_IMAGECROSS_H_
#define CODE_IMAGECROSS_H_

#include "zf_common_headfile.h"

typedef enum Cross_Type
{
    CROSS_NONE = 0,
    CROSS_BEGIN,
    CROSS_IN,
    CROSS_IN_CUT,
    CROSS_OUT_CUT,
    CROSS_OUT,
}Cross_Type;

extern Cross_Type cross_type;
extern bool cross_found_l, cross_found_r;
extern float distance;

uint8 CrossFindCorner(void);
uint8 CrossIdentify(void);
void EdgeDetection_Cross(void);



#endif /* CODE_IMAGECROSS_H_ */
