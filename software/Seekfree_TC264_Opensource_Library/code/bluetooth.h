/*
 * bluetooth.h
 *
 *  Created on: 2023��1��8��
 *      Author: L
 */

#ifndef CODE_BLUETOOTH_H_
#define CODE_BLUETOOTH_H_

#include "zf_driver_uart.h"
#include "zf_common_headfile.h"

#define BLUETOOTH       UART_2                 //����ģ��ʹ�õĴ��ں�
#define BLUETOOTH_TX    UART2_TX_P10_5         //����ģ�鷢������
#define BLUETOOTH_RX    UART2_RX_P10_6         //����ģ���������
#define BLUETOOTH_RATE  (115200)               //����ģ��Ĵ��ڲ�����

void UARTInit(void);
void seekfree_sendimg_03x(uart_index_enum uartn, uint8 *image, uint16 width, uint16 height);//��ͼ���͸������λ��

#endif /* CODE_BLUETOOTH_H_ */
