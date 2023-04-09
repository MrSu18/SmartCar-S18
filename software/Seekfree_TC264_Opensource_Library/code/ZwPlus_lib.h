/*
 * Copyright (C), 1988-1999, Xxxxxx Tech. Co., Ltd.
 * FileName: ZwPlus_lib.h
 * Description: 驺吾图传PLUS库函数
 * Change Logs:
  |Date          |Author       |Notes               |version
  |2022-10-29    |满心欢喜     |Initial build       |1.0.0
 */

#ifndef __ZWPLUS_LIB_H_
#define __ZWPLUS_LIB_H_

/* @include */
/* ... */
#include "zf_driver_uart.h"//逐飞的串口头文件
/* @define */
/* 图像尺寸 最大为120*188像素 */
#define    Zw_Image_H    120
#define    Zw_Image_W    188

/* 空类型定义 */
#define    Zw_NULL       0

/* UART数据发送宏定义    需要用户更改为自己的UART发送函数 */
#define    Zw_Putchar(Zw_data)    uart_write_byte(UART_2, Zw_data)

/* @typedef */
typedef    unsigned char    Zw_uint8;
typedef    unsigned short   Zw_uint16;

/* @Function declaration */
void Zw_SendImage(Zw_uint8* Zw_Image);

#endif

