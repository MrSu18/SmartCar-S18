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

void KEYInit(void);

#endif /* CODE_KEY_H_ */
