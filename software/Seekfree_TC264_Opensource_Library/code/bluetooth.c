/*
 * bluetooth.c
 *
 *  Created on: 2023��1��8��
 *      Author: L
 */
#include "bluetooth.h"
/***********************************************
* @brief : �����������ų�ʼ��
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
* @brief : �ض���printf
* @param : ch:��Ҫ��ӡ���ֽ�
*          f:������
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
* @brief : �ض���scanf
* @param : f:������
* @return: int
* @date  : 2023.1.8
* @author: L
************************************************/
int fgetc(FILE* f)
{
    return uart_read_byte(BLUETOOTH);
}
