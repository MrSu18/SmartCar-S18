/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2020,��ɿƼ�
 * All rights reserved.
 * ��������QQȺ����Ⱥ��824575535
 *
 * �����������ݰ�Ȩ������ɿƼ����У�δ��������������ҵ��;��
 * ��ӭ��λʹ�ò������������޸�����ʱ���뱣����ɿƼ��İ�Ȩ������
 *
 * @file       		�����
 * @company	   		�ɶ���ɿƼ����޹�˾
 * @author     		��ɿƼ�(QQ3184284598)
 * @version    		�鿴doc��version�ļ� �汾˵��
 * @Software 		ADS v1.2.2
 * @Target core		TC264D
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2020-3-23
 ********************************************************************************************************************/


#ifndef _SEEKFREE_FONT_h
#define _SEEKFREE_FONT_h

#include "common.h"



//-------������ɫ----------
#define RGB565_RED          	0xF800	//��ɫ
#define RGB565_BLUE         	0x001F  //��ɫ
#define RGB565_YELLOW       	0xFFE0	//��ɫ
#define RGB565_GREEN        	0x07E0	//��ɫ
#define RGB565_WHITE        	0xFFFF	//��ɫ
#define RGB565_BLACK        	0x0000	//��ɫ
#define RGB565_GRAY  			0X8430 	//��ɫ
#define RGB565_BROWN 			0XBC40 	//��ɫ
#define RGB565_PURPLE    		0XF81F	//��ɫ
#define RGB565_PINK    		0XFE19	//��ɫ


extern const uint8 tft_ascii[95][16];

extern const uint8 gImage_qq[3200];

extern const uint8 chinese_test[8][16];

extern const uint8 oled_6x8[][6];

extern const uint8 oled_8x16[];

extern const uint8 oled_16x16_chinese[][16];


#endif
