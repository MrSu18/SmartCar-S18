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

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ���������ͷͼ��������λ���鿴ͼ��
//  @param      uartn           ʹ�õĴ��ں�
//  @param      image           ��Ҫ���͵�ͼ���ַ
//  @param      width           ͼ�����
//  @param      height          ͼ�����
//  @return     void
//  @since      v1.0
//  Sample usage:seekfree_sendimg_03x(UART_2, mt9v03x_image[0], MT9V03X_W, MT9V03X_H);
//-------------------------------------------------------------------------------------------------------------------
void seekfree_sendimg_03x(uart_index_enum uartn, uint8 *image, uint16 width, uint16 height)
{
    uart_putchar(uartn,0x00);uart_putchar(uartn,0xff);uart_putchar(uartn,0x01);uart_putchar(uartn,0x01);//��������
    uart_putbuff(uartn, image, width*height);  //����ͼ��
}
