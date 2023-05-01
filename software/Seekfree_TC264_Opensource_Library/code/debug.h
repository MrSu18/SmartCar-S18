/*
 * debug.h
 *
 *  Created on: 2023年4月14日
 *      Author: 30516
 */

#ifndef CODE_DEBUG_H_
#define CODE_DEBUG_H_

void LCDShowPerImage(void);//LCD显示透视后的图像
void LCDShowFloatLine(myPoint_f* line,int len,const uint16 color);//LCD显示边线
void LCDShowUint8Line(myPoint* line,int len,const uint16 color);//LCD显示边线
void LCDDrowRow(uint8 row,const uint16 color);
void LCDDrowColumn(uint8 column,const uint16 color);
void LCDDrowPoint(uint8 row,uint8 column,const uint16 color);
void ShowImage(void);
void ShowLine(void);

#endif /* CODE_DEBUG_H_ */
