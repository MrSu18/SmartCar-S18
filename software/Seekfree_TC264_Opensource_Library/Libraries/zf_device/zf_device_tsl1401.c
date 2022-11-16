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
* �ļ�����          zf_device_tsl1401
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
/*********************************************************************************************************************
* ���߶��壺
*                  ------------------------------------
*                  ģ��ܽ�             ��Ƭ���ܽ�
*                  CLK                �鿴 zf_device_tsl1401.h �� TSL1401_CLK_PIN �궨��
*                  SI                 �鿴 zf_device_tsl1401.h �� TSL1401_SI_PIN �궨��
*                  AO[x]              �鿴 zf_device_tsl1401.h �� TSL1401_AO_PIN_BUFFER �궨��
*                  VCC                3.3V��Դ
*                  GND                ��Դ��
*                  ------------------------------------
********************************************************************************************************************/

#include "zf_common_debug.h"
#include "zf_driver_adc.h"
#include "zf_driver_delay.h"
#include "zf_driver_gpio.h"
#include "zf_driver_pit.h"
#include "zf_driver_uart.h"
#include "zf_device_tsl1401.h"

uint16 tsl1401_data[2][TSL1401_DATA_LEN];                                       // TSL1401 ���ݴ������

static uint8 tsl1401_init_state = 0;
vuint8 tsl1401_finish_flag;                                                     // TSL1401 ����׼��������־λ

//-------------------------------------------------------------------------------------------------------------------
// �������     TSL1401 ���� CCD ���ݲɼ�
// ����˵��     void
// ���ز���     void
// ʹ��ʾ��     tsl1401_collect_pit_handler();
// ��ע��Ϣ     �ú����� isr.c �ж�Ӧ TSL1401_PIT_INDEX ���жϷ���������
//-------------------------------------------------------------------------------------------------------------------
void tsl1401_collect_pit_handler (void)
{
    if(!tsl1401_init_state) return;

    uint8 i = 0;

    TSL1401_CLK(1);
    TSL1401_SI (0);
    TSL1401_CLK(0);
    TSL1401_SI (1);
    TSL1401_CLK(1);
    TSL1401_SI (0);

    for(i = 0; i < TSL1401_DATA_LEN; i ++)
    {
        TSL1401_CLK(0);
        tsl1401_data[0][i] = adc_convert(TSL1401_AO_PIN);
        tsl1401_data[1][i] = adc_convert(TSL1401_AO_PIN1);
        TSL1401_CLK(1);
    }

    tsl1401_finish_flag = 1;  // �ɼ���ɱ�־λ��1
}

//-------------------------------------------------------------------------------------------------------------------
// �������     TSL1401 ���� CCD ͼ��������λ���鿴ͼ��
// ����˵��     uart_n          ���ں�
// ����˵��     index           ��Ӧ������ĸ� TSL1401 [0-1]
// ���ز���     void
// ʹ��ʾ��     tsl1401_send_data(DEBUG_UART_INDEX, 1);
// ��ע��Ϣ     ���øú���ǰ���ȳ�ʼ������
//-------------------------------------------------------------------------------------------------------------------
void tsl1401_send_data (uart_index_enum uart_n, uint8 index)
{
    uint8 i;
    uart_write_byte(uart_n, 0x00);
    uart_write_byte(uart_n, 0xff);
    uart_write_byte(uart_n, 0x01);
    uart_write_byte(uart_n, 0x00);

    for(i=0; i<TSL1401_DATA_LEN; i++)
    {
        switch(TSL1401_AD_RESOLUTION)
        {
            case ADC_8BIT:  uart_write_byte(uart_n, (uint8)(tsl1401_data[index][i]));      break;
            case ADC_10BIT: uart_write_byte(uart_n, (uint8)(tsl1401_data[index][i] >> 2)); break;
            case ADC_12BIT: uart_write_byte(uart_n, (uint8)(tsl1401_data[index][i] >> 4)); break;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// �������     TSL1401 ���� CCD ��ʼ��
// ����˵��     void
// ���ز���     void
// ʹ��ʾ��     tsl1401_init();
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void tsl1401_init (void)
{
    adc_init(TSL1401_AO_PIN, TSL1401_AD_RESOLUTION);
    adc_init(TSL1401_AO_PIN1, TSL1401_AD_RESOLUTION);
    gpio_init(TSL1401_CLK_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);
    gpio_init(TSL1401_SI_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);
    pit_ms_init(TSL1401_PIT_INDEX, TSL1401_EXPOSURE_TIME);
    tsl1401_init_state = 1;
}

