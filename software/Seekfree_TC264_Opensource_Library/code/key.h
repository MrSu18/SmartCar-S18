/*
 * key.h
 *
 *  Created on: 2023��2��2��
 *      Author: L
 */

#ifndef CODE_KEY_H_
#define CODE_KEY_H_

#include "zf_common_headfile.h"
#include "zf_driver_gpio.h"

//��ȡ������ƽ�ĺ궨��
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

void KEYInit(void);                             //��ʼ����������
uint8 KEYScan(void);                            //ɨ���Ƿ��а���������
uint8 KeyGet(void);                             //ȷ�ϰ������²���ִֻ��һ��
uint8 PIDDisplay(uint8 key_num);                //ͨ������ѡ����ʾ�ĸ�PID�Ĳ���
void KeyPID(void);                              //ͨ����������PID����
void KeyTrack(void);                            //ͨ����������ǰհ�ͻ����ٶ�
void KeyImage(void);                            //ͨ������������ʾͼ��
void ShowPIDParameter(void);                    //��ʾ��ǰ����PID�Ĳ���
void ShowImageParameter(void);                  //��ʾ��ǰǰհ�ͻ����ٶ�
void KEYCtrl(void);                                 //�����������

#endif /* CODE_KEY_H_ */
