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

//-------------------------------------------------------------------------------------------------------------------
//  @brief      总钻风摄像头图像发送至上位机查看图像
//  @param      uartn           使用的串口号
//  @param      image           需要发送的图像地址
//  @param      width           图像的列
//  @param      height          图像的行
//  @return     void
//  @since      v1.0
//  Sample usage:seekfree_sendimg_03x(UART_2, mt9v03x_image[0], MT9V03X_W, MT9V03X_H);
//-------------------------------------------------------------------------------------------------------------------
void seekfree_sendimg_03x(uart_index_enum uartn, uint8 *image, uint16 width, uint16 height)
{
    uart_putchar(uartn,0x00);uart_putchar(uartn,0xff);uart_putchar(uartn,0x01);uart_putchar(uartn,0x01);//发送命令
    uart_putbuff(uartn, image, width*height);  //发送图像
}
