/*********************************************************************************************************************
* TC264 Opensourec Library ����TC264 ��Դ�⣩��һ�����ڹٷ� SDK �ӿڵĵ�������Դ��
* Copyright (c) 2022 SEEKFREE ��ɿƼ�
*
* ���ļ��� TC264 ��Դ���һ����
*
* TC264 ��Դ�� ��������
* �����Ը��������������ᷢ���� GPL��GNU General Public License���� GNUͨ�ù������֤��������
* �� GPL �ĵ�3�棨�� GPL3.0������ѡ��ģ��κκ����İ汾�����·�����/���޸���
*
* ����Դ��ķ�����ϣ�����ܷ������ã�����δ�������κεı�֤
* ����û�������������Ի��ʺ��ض���;�ı�֤
* ����ϸ����μ� GPL
*
* ��Ӧ�����յ�����Դ���ͬʱ�յ�һ�� GPL �ĸ���
* ���û�У������<https://www.gnu.org/licenses/>
*
* ����ע����
* ����Դ��ʹ�� GPL3.0 ��Դ���֤Э�� �����������Ϊ���İ汾
* �������Ӣ�İ��� libraries/doc �ļ����µ� GPL3_permission_statement.txt �ļ���
* ���֤������ libraries �ļ����� �����ļ����µ� LICENSE �ļ�
* ��ӭ��λʹ�ò����������� ���޸�����ʱ���뱣����ɿƼ��İ�Ȩ����������������
*
* �ļ�����          zf_driver_uart
* ��˾����          �ɶ���ɿƼ����޹�˾
* �汾��Ϣ          �鿴 libraries/doc �ļ����� version �ļ� �汾˵��
* ��������          ADS v1.8.0
* ����ƽ̨          TC264D
* ��������          https://seekfree.taobao.com/
*
* �޸ļ�¼
* ����              ����                ��ע
* 2022-09-15       pudding            first version
********************************************************************************************************************/

#ifndef _zf_driver_uart_h_
#define _zf_driver_uart_h_

#include "ifxAsclin_Asc.h"
#include "zf_common_typedef.h"

#define UART0_TX_BUFFER_SIZE 256        // ���崮��0���ͻ�������С
#define UART0_RX_BUFFER_SIZE 16         // ���崮��0���ջ�������С

#define UART1_TX_BUFFER_SIZE 256
#define UART1_RX_BUFFER_SIZE 16

#define UART2_TX_BUFFER_SIZE 256
#define UART2_RX_BUFFER_SIZE 16

#define UART3_TX_BUFFER_SIZE 256
#define UART3_RX_BUFFER_SIZE 16


typedef enum            // ö�ٴ������� ��ö�ٶ��岻�����û��޸�
{
    UART0_TX_P14_0,     // ����0 �������ſ�ѡ��Χ
    UART0_TX_P14_1,
    UART0_TX_P15_2,
    UART0_TX_P15_3,

    UART1_TX_P02_2,     // ����1 �������ſ�ѡ��Χ
    UART1_TX_P11_12,
    UART1_TX_P15_0,
    UART1_TX_P15_1,
    UART1_TX_P15_4,
    UART1_TX_P15_5,
    UART1_TX_P20_10,
    UART1_TX_P33_12,
    UART1_TX_P33_13,

    UART2_TX_P02_0,     // ����2 �������ſ�ѡ��Χ
    UART2_TX_P10_5,
    UART2_TX_P14_2,
    UART2_TX_P14_3,
    UART2_TX_P33_8,
    UART2_TX_P33_9,

    UART3_TX_P00_0,     // ����3 �������ſ�ѡ��Χ
    UART3_TX_P00_1,
    UART3_TX_P15_6,
    UART3_TX_P15_7,
    UART3_TX_P20_0,
    UART3_TX_P20_3,
    UART3_TX_P21_7,
}uart_tx_pin_enum;


typedef enum            // ö�ٴ������� ��ö�ٶ��岻�����û��޸�
{

    UART0_RX_P14_1,     // ����0 �������ſ�ѡ��Χ
    UART0_RX_P15_3,

    UART1_RX_P02_3,     // ����1 �������ſ�ѡ��Χ
    UART1_RX_P11_10,
    UART1_RX_P15_1,
    UART1_RX_P15_5,
    UART1_RX_P20_9,
    UART1_RX_P33_13,

    UART2_RX_P02_0,     // ����2 �������ſ�ѡ��Χ
    UART2_RX_P02_1,
    UART2_RX_P10_6,
    UART2_RX_P14_3,
    UART2_RX_P33_8,

    UART3_RX_P00_1,     // ����3 �������ſ�ѡ��Χ
    UART3_RX_P15_7,
    UART3_RX_P20_3,
    UART3_RX_P21_6,
}uart_rx_pin_enum;


typedef enum            // ö�ٴ��ں� ��ö�ٶ��岻�����û��޸�
{
    UART_0,
    UART_1,
    UART_2,
    UART_3,
}uart_index_enum;

//��������handle����
extern IfxAsclin_Asc uart0_handle;
extern IfxAsclin_Asc uart1_handle;
extern IfxAsclin_Asc uart2_handle;
extern IfxAsclin_Asc uart3_handle;

//====================================================���� ��������====================================================
void    uart_write_byte_wait                (uart_index_enum uart_n, const uint8 dat);
void    uart_write_byte                     (uart_index_enum uartn, const uint8 dat);
void    uart_write_buffer                   (uart_index_enum uartn, const uint8 *buff, uint32 len);
void    uart_write_string                   (uart_index_enum uartn, const char *str);

uint8   uart_read_byte                      (uart_index_enum uartn);
uint8   uart_query_byte                     (uart_index_enum uartn, uint8 *dat);

void    uart_tx_interrupt                   (uart_index_enum uartn, uint32 status);
void    uart_rx_interrupt                   (uart_index_enum uartn, uint32 status);

void    uart_init                           (uart_index_enum uartn, uint32 baud, uart_tx_pin_enum tx_pin, uart_rx_pin_enum rx_pin);
//====================================================���� ��������====================================================

//=================================================���ݾɰ汾��Դ��ӿ�����=================================================
#ifdef  COMPATIBLE_WITH_OLDER_VERSIONS
#define uart_putchar(uart_n, dat)           (uart_write_byte((uart_n), (dat)))
#define uart_putbuff(uart_n, buff, len)     (uart_write_buffer((uart_n), (buff), (len)))
#define uart_putstr(uart_n, str)            (uart_write_string((uart_n), (str)))

#define uart_getchar(uart_n, dat)           (*(dat) = uart_read_byte((uart_n)))
#define uart_query(uart_n, dat)             (uart_query_byte((uart_n), (dat)))

#define uart_tx_irq(uart_n, status)         (uart_tx_interrupt((uart_n), (status)))
#define uart_rx_irq(uart_n, status)         (uart_rx_interrupt((uart_n), (status)))
#endif
//=================================================���ݾɰ汾��Դ��ӿ�����=================================================

#endif
