/*
 * debug.h
 *
 *  Created on: 2023年4月14日
 *      Author: 30516
 */

#ifndef CODE_DEBUG_H_
#define CODE_DEBUG_H_

#include "ImageTrack.h"
#include "zf_driver_pwm.h"

#define GREEN    ATOM1_CH6_P23_1
#define RED      ATOM1_CH1_P22_0
#define BLUE     ATOM1_CH0_P22_1
#define BEER     P21_2

void LCDShowPerImage(void);//LCD显示透视后的图像
void LCDShowFloatLine(myPoint_f* line,int len,const uint16 color);//LCD显示边线
void LCDShowUint8Line(myPoint* line,int len,const uint16 color);//LCD显示边线
void LCDDrowRow(uint8 row,const uint16 color);
void LCDDrowColumn(uint8 column,const uint16 color);
void LCDDrowPoint(uint8 row,uint8 column,const uint16 color);
void ShowImage(void);
void ShowLine(void);
void LedInit(void);
void ColorOfRGB(int green_pwm,int red_pwm,int blue_pwm);
void LedSet (uint8 R,uint8 G,uint8 B);
#endif /* CODE_DEBUG_H_ */
