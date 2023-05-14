/*
 * KeyMenu.h
 *
 *  Created on: 2023��5��14��
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

//��ȡ������ƽ�ĺ궨��
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

void KEYInit(void);                             //��ʼ����������
uint8 KEYScan(void);                            //ɨ���Ƿ��а���������
uint8 KeyGet(void);                             //ȷ�ϰ������²���ִֻ��һ��
void ShowFunction(uint8 page);                  //��ʾ��Ӧ�˵�ҳ
void KeyCtrl(void);                             //�����������
void SubParameter(void);                        //������
void AddParameter(void);                        //������
void EnterKey(uint8* exit_flag);                //ȷ�ϼ�ʵ�ֶ�Ӧ����
void WriteToFlash(void);                        //����Ҫ�޸ĵ�ֵд��Flash
void ReadFromFlash(void);                       //��Flash��ȡֵ����Ӧ�ı���

#endif /* CODE_KEYMENU_H_ */
