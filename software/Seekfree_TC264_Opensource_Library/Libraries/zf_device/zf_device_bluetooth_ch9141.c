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
* �ļ�����          zf_device_bluetooth_ch9141
* ��˾����          �ɶ���ɿƼ����޹�˾
* �汾��Ϣ          �鿴 libraries/doc �ļ����� version �ļ� �汾˵��
* ��������          ADS v1.8.0
* ����ƽ̨          TC264D
* ��������          https://seekfree.taobao.com/
*
* �޸ļ�¼
* ����             ����                ��ע
* 2022-09-15      pudding            first version
********************************************************************************************************************/
/*********************************************************************************************************************
* ���߶��壺
*                 ------------------------------------
*                 ����ת����           ��Ƭ��
*                 RX                 �鿴zf_device_bluetooth_ch9141.h�ļ��е�BLUETOOTH_CH9141_TX_PIN�궨��
*                 TX                 �鿴zf_device_bluetooth_ch9141.h�ļ��е�BLUETOOTH_CH9141_RX_PIN�궨��
*                 RTS                �鿴zf_device_bluetooth_ch9141.h�ļ��е�BLUETOOTH_CH9141_RTS_PIN�궨��
*                 CTS                ����
*                 CMD                ���ջ�������
*                 ------------------------------------
*********************************************************************************************************************/

#include "zf_common_debug.h"
#include "zf_common_fifo.h"
#include "zf_driver_gpio.h"
#include "zf_driver_uart.h"
#include "zf_driver_delay.h"
#include "zf_device_bluetooth_ch9141.h"

static  fifo_struct     bluetooth_ch9141_fifo;
static  uint8           bluetooth_ch9141_buffer[BLUETOOTH_CH9141_BUFFER_SIZE];  // ���ݴ������

static  uint8           bluetooth_ch9141_data;

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ����ת����ģ�� �����жϻص�����
//  @param      void
//  @return     void
//  Sample usage:
//  @note       �ú�����ISR�ļ� �����жϳ��򱻵���
//-------------------------------------------------------------------------------------------------------------------
void bluetooth_ch9141_uart_callback (void)
{
    uart_query_byte(BLUETOOTH_CH9141_INDEX, &bluetooth_ch9141_data);            // ��ȡ��������
    fifo_write_buffer(&bluetooth_ch9141_fifo, &bluetooth_ch9141_data, 1);       // ���� FIFO
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       ����ת����ģ�� ��������
// @param       data            8bit ����
// @return      uint32          ʣ�෢�ͳ���
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint32 bluetooth_ch9141_send_byte (const uint8 data)
{
    uint16 time_count = 0;
    while(gpio_get_level(BLUETOOTH_CH9141_RTS_PIN))                             // ���RTSΪ�͵�ƽ���������������
    {
        if(BLUETOOTH_CH9141_TIMEOUT_COUNT < time_count ++)
            return 1;                                                           // ģ��æ,�������ǰ����ʹ��while�ȴ� �����ʹ�ú���ע�͵�while�ȴ�����滻��if���
        system_delay_ms(1);
    }
    uart_write_byte(BLUETOOTH_CH9141_INDEX, data);                              // ������������

    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ����ת����ģ�� ���ͺ���
//  @param      buff            ��Ҫ���͵����ݵ�ַ
//  @param      len             ���ͳ���
//  @return     uint32          ʣ��δ���͵��ֽ���
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint32 bluetooth_ch9141_send_buff (const uint8 *buff, uint32 len)
{
    uint16 time_count = 0;
    while(30 < len)
    {
        time_count = 0;
        while(gpio_get_level(BLUETOOTH_CH9141_RTS_PIN) && BLUETOOTH_CH9141_TIMEOUT_COUNT > time_count ++) // ���RTSΪ�͵�ƽ���������������
            system_delay_ms(1);
        if(BLUETOOTH_CH9141_TIMEOUT_COUNT <= time_count)
            return len;                                                         // ģ��æ,�������ǰ����ʹ��while�ȴ� �����ʹ�ú���ע�͵�while�ȴ�����滻��if���
        uart_write_buffer(BLUETOOTH_CH9141_INDEX, buff, 30);

        buff += 30;                                                             // ��ַƫ��
        len -= 30;                                                              // ����
    }

    time_count = 0;
    while(gpio_get_level(BLUETOOTH_CH9141_RTS_PIN) && BLUETOOTH_CH9141_TIMEOUT_COUNT > time_count ++)     // ���RTSΪ�͵�ƽ���������������
        system_delay_ms(1);
    if(time_count >= BLUETOOTH_CH9141_TIMEOUT_COUNT)
        return len;                                                             // ģ��æ,�������ǰ����ʹ��while�ȴ� �����ʹ�ú���ע�͵�while�ȴ�����滻��if���
    uart_write_buffer(BLUETOOTH_CH9141_INDEX, buff, len);                       // ������������

    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       ����ת����ģ�� �����ַ���
// @param       *str            Ҫ���͵��ַ�����ַ
// @return      uint32          ʣ�෢�ͳ���
// Sample usage:                bluetooth_ch9141_send_string("Believe in yourself.");
//-------------------------------------------------------------------------------------------------------------------
uint32 bluetooth_ch9141_send_string (const char *str)
{
    uint16 time_count = 0;
    uint32 len = strlen(str);
    while(30 < len)
    {
        time_count = 0;
        while(gpio_get_level(BLUETOOTH_CH9141_RTS_PIN))                         // ���RTSΪ�͵�ƽ���������������
        {
            if(BLUETOOTH_CH9141_TIMEOUT_COUNT > time_count ++)
                return len;                                                     // ģ��æ,�������ǰ����ʹ��while�ȴ� �����ʹ�ú���ע�͵�while�ȴ�����滻��if���
            system_delay_ms(1);
        }
        uart_write_buffer(BLUETOOTH_CH9141_INDEX, (const uint8 *)str, 30);

        str += 30;                                                              // ��ַƫ��
        len -= 30;                                                              // ����
    }

    time_count = 0;
    while(gpio_get_level(BLUETOOTH_CH9141_RTS_PIN))                             // ���RTSΪ�͵�ƽ���������������
    {
        if(BLUETOOTH_CH9141_TIMEOUT_COUNT > time_count ++)
            return len;                                                         // ģ��æ,�������ǰ����ʹ��while�ȴ� �����ʹ�ú���ע�͵�while�ȴ�����滻��if���
        system_delay_ms(1);
    }
    uart_write_buffer(BLUETOOTH_CH9141_INDEX, (const uint8 *)str, len);         // ������������

    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       ����ת����ģ�� ��������ͷͼ������λ���鿴ͼ��
// @param       *image_addr     ��Ҫ���͵�ͼ���ַ
// @param       image_size      ͼ��Ĵ�С
// @return      void
// Sample usage:                bluetooth_ch9141_send_image(&mt9v03x_image[0][0], MT9V03X_IMAGE_SIZE);
//-------------------------------------------------------------------------------------------------------------------
void bluetooth_ch9141_send_image (const uint8 *image_addr, uint32 image_size)
{
    extern uint8 camera_send_image_frame_header[4];
    bluetooth_ch9141_send_buff(camera_send_image_frame_header, 4);
    bluetooth_ch9141_send_buff((uint8 *)image_addr, image_size);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       ����ת����ģ�� ��ȡ����
// @param       buff            �洢�����ݵ�ַ
// @param       len             ����
// @return      uint32          ʵ�ʶ�ȡ�ֽ���
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint32 bluetooth_ch9141_read_buff (uint8 *buff, uint32 len)
{
    uint32 data_len = len;
    fifo_read_buffer(&bluetooth_ch9141_fifo, buff, &data_len, FIFO_READ_AND_CLEAN);
    return data_len;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       ����ת����ģ�� ��ʼ��
// @param       mode            ����ģʽ MASTER_MODE(����)����SLAVE_MODE(�ӻ�)
// @return      uint8           ��ʼ��״̬ 0-�ɹ� 1-ʧ��
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint8 bluetooth_ch9141_init (void)
{
    set_wireless_type(BLUETOOTH_CH9141, bluetooth_ch9141_uart_callback);

    fifo_init(&bluetooth_ch9141_fifo, FIFO_DATA_8BIT, bluetooth_ch9141_buffer, BLUETOOTH_CH9141_BUFFER_SIZE);
    // ������ʹ�õĲ�����Ϊ115200 Ϊ����ת����ģ���Ĭ�ϲ����� ����������������ʹ����λ���޸�ģ�����
    gpio_init(BLUETOOTH_CH9141_RTS_PIN, GPI, 1, GPI_PULL_UP);                   // ��ʼ����������
    uart_init(BLUETOOTH_CH9141_INDEX, BLUETOOTH_CH9141_BUAD_RATE, BLUETOOTH_CH9141_RX_PIN, BLUETOOTH_CH9141_TX_PIN);
    uart_rx_interrupt(BLUETOOTH_CH9141_INDEX, 1);

    return 0;
}
