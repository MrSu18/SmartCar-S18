/*
 * KeyMenu.h
 *
 *  Created on: 2023年5月14日
 *      Author: L
 */

#ifndef CODE_KEYMENU_H_
#define CODE_KEYMENU_H_

#include "zf_common_headfile.h"
#include "zf_driver_gpio.h"

typedef struct Menu
{
        uint8 page;
        int8 updown;
        uint8 enter;
}Menu;

//获取按键电平的宏定义
#define  KEY1  gpio_get_level(P33_10)
#define  KEY2  gpio_get_level(P33_11)
#define  KEY3  gpio_get_level(P33_12)
#define  KEY4  gpio_get_level(P33_13)
#define  KEY5  gpio_get_level(P32_4)
#define  KEY6  gpio_get_level(P33_9)

#define  KEY_UP         1
#define  KEY_ENTER      2
#define  KEY_LEFT       3
#define  KEY_RIGHT      4
#define  KEY_DOWN       5
#define  KEY_OWN        6

#define  PRESSDOWN      0
#define  LOOSE          1

extern uint8 per_image_flag,gray_image_flag;
extern uint8 edgeline_flag,c_line_flag,per_edgeline_flag;

void KEYInit(void);                             //初始化按键引脚
uint8 KEYScan(void);                            //扫描是否有按键被按下
uint8 KeyGet(void);                             //确认按键按下并且只执行一次
void ShowFunction(uint8 page);                  //显示对应菜单页
void KeyCtrl(void);                             //按键整体控制
void SubParameter(void);                        //参数减
void AddParameter(void);                        //参数加
void EnterKey(uint8* exit_flag);                //确认键实现对应功能
void WriteToFlash(void);                        //将需要修改的值写入Flash
void ReadFromFlash(void);                       //从Flash获取值到对应的变量

#endif /* CODE_KEYMENU_H_ */
