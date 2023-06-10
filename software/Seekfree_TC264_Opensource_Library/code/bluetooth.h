/*
 * bluetooth.h
 *
 *  Created on: 2023年1月8日
 *      Author: L
 */

#ifndef CODE_BLUETOOTH_H_
#define CODE_BLUETOOTH_H_

#include "zf_driver_uart.h"
#include "zf_common_headfile.h"

#define BLUETOOTH       UART_2                 //蓝牙模块使用的串口号
#define BLUETOOTH_TX    UART2_TX_P10_5         //蓝牙模块发送引脚
#define BLUETOOTH_RX    UART2_RX_P10_6         //蓝牙模块接收引脚
#define BLUETOOTH_RATE  (115200)               //蓝牙模块的串口波特率

void UARTInit(void);
void seekfree_sendimg_03x(uart_index_enum uartn, uint8 *image, uint16 width, uint16 height);//把图像发送给逐飞上位机

#endif /* CODE_BLUETOOTH_H_ */
