/*
 * bluetooth.c
 *
 *  Created on: 2023年1月8日
 *      Author: L
 */
#include "bluetooth.h"
/***********************************************
* @brief : 蓝牙串口引脚初始化
* @param : void
* @return: void
* @date  : 2023.1.8
* @author: L
************************************************/
void UARTInit(void)
{
    uart_init(BLUETOOTH,BLUETOOTH_RATE,BLUETOOTH_TX,BLUETOOTH_RX);
}
/***********************************************
* @brief : 重定向printf
* @param : ch:需要打印的字节
*          f:数据流
* @return: int
* @date  : 2023.1.8
* @author: L
************************************************/
int fputc(int ch,FILE* f)
{
    uart_write_byte(BLUETOOTH,(char)ch);
    return ch;
}
/***********************************************
* @brief : 重定向scanf
* @param : f:数据流
* @return: int
* @date  : 2023.1.8
* @author: L
************************************************/
int fgetc(FILE* f)
{
    return uart_read_byte(BLUETOOTH);
}
