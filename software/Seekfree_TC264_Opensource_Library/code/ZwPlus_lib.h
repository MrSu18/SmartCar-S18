/*
 * Copyright (C), 1988-1999, Xxxxxx Tech. Co., Ltd.
 * FileName: ZwPlus_lib.h
 * Description: ����ͼ��PLUS�⺯��
 * Change Logs:
  |Date          |Author       |Notes               |version
  |2022-10-29    |���Ļ�ϲ     |Initial build       |1.0.0
 */

#ifndef __ZWPLUS_LIB_H_
#define __ZWPLUS_LIB_H_

/* @include */
/* ... */
#include "zf_driver_uart.h"//��ɵĴ���ͷ�ļ�
/* @define */
/* ͼ��ߴ� ���Ϊ120*188���� */
#define    Zw_Image_H    120
#define    Zw_Image_W    188

/* �����Ͷ��� */
#define    Zw_NULL       0

/* UART���ݷ��ͺ궨��    ��Ҫ�û�����Ϊ�Լ���UART���ͺ��� */
#define    Zw_Putchar(Zw_data)    uart_write_byte(UART_2, Zw_data)

/* @typedef */
typedef    unsigned char    Zw_uint8;
typedef    unsigned short   Zw_uint16;

/* @Function declaration */
void Zw_SendImage(Zw_uint8* Zw_Image);

#endif

