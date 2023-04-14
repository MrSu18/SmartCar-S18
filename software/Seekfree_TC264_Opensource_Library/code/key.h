/*
 * key.h
 *
 *  Created on: 2023年2月2日
 *      Author: L
 */

#ifndef CODE_KEY_H_
#define CODE_KEY_H_

#include "zf_common_headfile.h"
#include "zf_driver_gpio.h"

//获取按键电平的宏定义
#define  KEY1  gpio_get_level(P33_10)
#define  KEY2  gpio_get_level(P33_11)
#define  KEY3  gpio_get_level(P33_12)
#define  KEY4  gpio_get_level(P33_13)
#define  KEY5  gpio_get_level(P32_4)

#define  KEY_UP         1
#define  KEY_DOWN       2
#define  KEY_LEFT       3
#define  KEY_RIGHT      4
#define  KEY_ENTER      5

#define  PRESSDOWN      1
#define  LOOSE          0

extern uint8 binary_image_flag,gray_image_flag;
extern uint8 edgeline_flag,c_line_flag,per_edgeline_flag;

void KEYInit(void);                             //初始化按键引脚
uint8 KEYScan(void);                            //扫描是否有按键被按下
uint8 KeyGet(void);                             //确认按键按下并且只执行一次
uint8 PIDDisplay(uint8 key_num);                //通过按键选择显示哪个PID的参数
void KeyPID(void);                              //通过按键调节PID参数
void KeyTrack(void);                            //通过按键调节前瞻和基础速度
void KeyImage(void);                            //通过按键控制显示图像
void ShowPIDParameter(void);                    //显示当前所有PID的参数
void ShowImageParameter(void);                  //显示当前前瞻和基础速度
void KEYCtrl(void);                                 //按键整体控制

#endif /* CODE_KEY_H_ */
