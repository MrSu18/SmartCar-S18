/*
 * Menu.h
 *
 *  Created on: 2023年7月7日
 *      Author: L
 */
#ifndef CODE_MENU_H_
#define CODE_MENU_H_

#include "zf_common_headfile.h"
#include "zf_driver_gpio.h"

//获取按键电平的宏定义
#define  KEY1  gpio_get_level(P33_10)
#define  KEY2  gpio_get_level(P33_11)
#define  KEY3  gpio_get_level(P33_12)
#define  KEY4  gpio_get_level(P33_13)
#define  KEY5  gpio_get_level(P32_4)
#define  KEY6  gpio_get_level(P33_8)
#define  KEY7  gpio_get_level(P33_9)
#define  KEY_LOOSE      0
#define  KEY_UP         1
#define  KEY_ENTER      2
#define  KEY_LEFT       3
#define  KEY_RIGHT      4
#define  KEY_DOWN       5
#define  KEY_TOP        6
#define  KEY_LOW        7
//按键按下松开的宏定义
#define PRESS  0//按键按下
#define LOOSE  1//按键松开

#define Line1  0
#define Line2  1
#define Line3  2
#define Line4  3
#define Line5  4
#define Line6  5
#define Line7  6
#define Line8  7
#define Line9  8
#define Line10  9
#define Line11  10
#define Line12  11
#define Line13  12
#define Line14  13

extern short select,my_sel;
extern int page,key_N;
extern uint8 per_image_flag,gray_image_flag;
extern uint8 edgeline_flag,c_line_flag,per_edgeline_flag;
extern int16 Change_EXP_TIME_DEF;


void KEYInit(void);                             //初始化按键引脚
uint8 KEYScan(void);                            //扫描是否有按键被按下
uint8 KeyGet(void);                             //确认按键按下并且只执行一次
void KeyStateMachine(void);                     //按键状态机
void WakeUpScreen(void);
void WriteToFlash(uint32 page,uint32 read_flag);
void ReadFromFlash(void);

#endif /* CODE_MENU_H_ */
