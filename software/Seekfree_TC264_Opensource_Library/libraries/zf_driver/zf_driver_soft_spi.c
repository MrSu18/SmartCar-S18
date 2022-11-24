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
* �ļ�����          zf_driver_soft_spi
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

#include "zf_common_debug.h"
#include "zf_driver_soft_spi.h"

//-------------------------------------------------------------------------------------------------------------------
// �������     ��� SPI ��ʱ
// ����˵��     void
// ���ز���     void
// ʹ��ʾ��     soft_spi_delay(1);
// ��ע��Ϣ     �ڲ�����
//-------------------------------------------------------------------------------------------------------------------
#define soft_spi_delay(x)  for(uint32 i = x; i --; )

//-------------------------------------------------------------------------------------------------------------------
// �������     ��� SPI 8bit ���ݶ�д
// ����˵��     *soft_spi_obj   ��� SPI ָ����Ϣ��Žṹ���ָ��
// ����˵��     data            ����
// ���ز���     uint8           ��ȡ������
// ʹ��ʾ��     soft_spi_8bit_data_handler(soft_spi_obj, 1);
// ��ע��Ϣ     �ڲ�����
//-------------------------------------------------------------------------------------------------------------------
static uint8 soft_spi_8bit_data_handler (soft_spi_info_struct *soft_spi_obj, const uint8 data)
{
    uint8 temp = 0;
    uint8 write_data = data;
    uint8 read_data = 0;

    if(soft_spi_obj->config.use_cs)
    {
        gpio_low(soft_spi_obj->cs_pin);
    }

    if(0 == soft_spi_obj->config.mode || 1 == soft_spi_obj->config.mode)        // CPOL = 0 SCK ���е͵�ƽ
    {
        gpio_low(soft_spi_obj->sck_pin);
    }
    else                                                                        // CPOL = 1 SCK ���иߵ�ƽ
    {
        gpio_high(soft_spi_obj->sck_pin);
    }

    if(0 == soft_spi_obj->config.mode % 2)                                      // CPHA = 0 ��һ�����ز���
    {
        for(temp = 8; temp > 0; temp --)
        {
            if(0x80 & write_data)
            {
                gpio_high(soft_spi_obj->mosi_pin);
            }
            else
            {
                gpio_low(soft_spi_obj->mosi_pin);
            }
            soft_spi_delay(soft_spi_obj->delay);
            gpio_toggle_level(soft_spi_obj->sck_pin);
            write_data = write_data << 1;
            read_data = read_data << 1;
            if(soft_spi_obj->config.use_miso)
            {
                read_data |= gpio_get_level(soft_spi_obj->miso_pin);
            }
            soft_spi_delay(soft_spi_obj->delay);
            gpio_toggle_level(soft_spi_obj->sck_pin);
        }
    }
    else                                                                        // CPHA = 1 �ڶ������ز���
    {
        for(temp = 8; 0 < temp; temp --)
        {
            gpio_toggle_level(soft_spi_obj->sck_pin);
            if(0x80 & write_data)
            {
                gpio_high(soft_spi_obj->mosi_pin);
            }
            else
            {
                gpio_low(soft_spi_obj->mosi_pin);
            }
            soft_spi_delay(soft_spi_obj->delay);
            gpio_toggle_level(soft_spi_obj->sck_pin);
            write_data = write_data << 1;
            read_data = read_data << 1;
            if(soft_spi_obj->config.use_miso)
            {
                read_data |= gpio_get_level(soft_spi_obj->miso_pin);
            }
            soft_spi_delay(soft_spi_obj->delay);
        }
    }

    if(soft_spi_obj->config.use_cs)
    {
        gpio_high(soft_spi_obj->cs_pin);
    }
    return read_data;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ��� SPI 16bit ���ݶ�д
// ����˵��     *soft_spi_obj   ��� SPI ָ����Ϣ��Žṹ���ָ��
// ����˵��     data            ����
// ���ز���     uint16          ��ȡ������
// ʹ��ʾ��     soft_spi_16bit_data_handler(soft_spi_obj, 1);
// ��ע��Ϣ     �ڲ�����
//-------------------------------------------------------------------------------------------------------------------
static uint16 soft_spi_16bit_data_handler (soft_spi_info_struct *soft_spi_obj, const uint16 data)
{
    uint8 temp = 0;
    uint16 write_data = data;
    uint16 read_data = 0;

    if(soft_spi_obj->config.use_cs)
    {
        gpio_low(soft_spi_obj->cs_pin);
    }

    if(0 == soft_spi_obj->config.mode || 1 == soft_spi_obj->config.mode)        // CPOL = 0 SCK ���е͵�ƽ
    {
        gpio_low(soft_spi_obj->sck_pin);
    }
    else                                                                        // CPOL = 1 SCK ���иߵ�ƽ
    {
        gpio_high(soft_spi_obj->sck_pin);
    }

    if(0 == soft_spi_obj->config.mode % 2)                                      // CPHA = 0 ��һ�����ز���
    {
        for(temp = 16; 0 < temp; temp --)
        {
            if(0x8000 & write_data)
            {
                gpio_high(soft_spi_obj->mosi_pin);
            }
            else
            {
                gpio_low(soft_spi_obj->mosi_pin);
            }
            soft_spi_delay(soft_spi_obj->delay);
            gpio_toggle_level(soft_spi_obj->sck_pin);
            write_data = write_data << 1;
            read_data = read_data << 1;
            if(soft_spi_obj->config.use_miso)
            {
                read_data |= gpio_get_level(soft_spi_obj->miso_pin);
            }
            soft_spi_delay(soft_spi_obj->delay);
            gpio_toggle_level(soft_spi_obj->sck_pin);
        }
    }
    else                                                                        // CPHA = 1 �ڶ������ز���
    {
        for(temp = 16; 0 < temp; temp --)
        {
            gpio_toggle_level(soft_spi_obj->sck_pin);
            if(write_data & 0x8000)
            {
                gpio_high(soft_spi_obj->mosi_pin);
            }
            else
            {
                gpio_low(soft_spi_obj->mosi_pin);
            }
            soft_spi_delay(soft_spi_obj->delay);
            gpio_toggle_level(soft_spi_obj->sck_pin);
            write_data = write_data << 1;
            read_data = read_data << 1;
            if(soft_spi_obj->config.use_miso)
            {
                read_data |= gpio_get_level(soft_spi_obj->miso_pin);
            }
            soft_spi_delay(soft_spi_obj->delay);
        }
    }

    if(soft_spi_obj->config.use_cs)
    {
        gpio_high(soft_spi_obj->cs_pin);
    }
    return read_data;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ��� SPI �ӿ�д 8bit ����
// ����˵��     *soft_spi_obj   ��� SPI ָ����Ϣ��Žṹ���ָ��
// ����˵��     data            ���͵�����
// ���ز���     void
// ʹ��ʾ��     soft_spi_write_8bit(&soft_spi_obj, 1);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_write_8bit (soft_spi_info_struct *soft_spi_obj, const uint8 data)
{
    soft_spi_8bit_data_handler(soft_spi_obj, data);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ��� SPI �ӿ�д 8bit ����
// ����˵��     *soft_spi_obj   ��� SPI ָ����Ϣ��Žṹ���ָ��
// ����˵��     *data           ���ݴ�Ż�����
// ����˵��     len             ����������
// ���ز���     void
// ʹ��ʾ��     soft_spi_write_8bit_array(&soft_spi_obj, buf, 16);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_write_8bit_array (soft_spi_info_struct *soft_spi_obj, const uint8 *data, uint32 len)
{
    while(len --)
    {
        soft_spi_8bit_data_handler(soft_spi_obj, *data ++);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ��� SPI �ӿ�д 16bit ����
// ����˵��     *soft_spi_obj   ��� SPI ָ����Ϣ��Žṹ���ָ��
// ����˵��     data            ���͵�����
// ���ز���     void
// ʹ��ʾ��     soft_spi_write_16bit(&soft_spi_obj, 1);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_write_16bit (soft_spi_info_struct *soft_spi_obj, uint16 data)
{
    soft_spi_16bit_data_handler(soft_spi_obj, data);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ��� SPI �ӿ�д 16bit ����
// ����˵��     *soft_spi_obj   ��� SPI ָ����Ϣ��Žṹ���ָ��
// ����˵��     *data           ���ݴ�Ż�����
// ����˵��     len             ����������
// ���ز���     void
// ʹ��ʾ��     soft_spi_write_16bit_array(&soft_spi_obj, buf, 16);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_write_16bit_array (soft_spi_info_struct *soft_spi_obj, const uint16 *data, uint32 len)
{
    while(len --)
    {
        soft_spi_16bit_data_handler(soft_spi_obj, *data ++);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ��� SPI �ӿ��򴫸����ļĴ���д 8bit ����
// ����˵��     *soft_spi_obj   ��� SPI ָ����Ϣ��Žṹ���ָ��
// ����˵��     register_name   �Ĵ�����ַ
// ����˵��     data            ���͵�����
// ���ز���     void
// ʹ��ʾ��     soft_spi_write_8bit_register(&soft_spi_obj, 1, 1);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_write_8bit_register (soft_spi_info_struct *soft_spi_obj, const uint8 register_name, const uint8 data)
{
    soft_spi_8bit_data_handler(soft_spi_obj, register_name);
    soft_spi_8bit_data_handler(soft_spi_obj, data);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ��� SPI �ӿ��򴫸����ļĴ���д 8bit ����
// ����˵��     *soft_spi_obj   ��� SPI ָ����Ϣ��Žṹ���ָ��
// ����˵��     register_name   �Ĵ�����ַ
// ����˵��     *data           ���ݴ�Ż�����
// ����˵��     len             ����������
// ���ز���     void
// ʹ��ʾ��     soft_spi_write_8bit_registers(&soft_spi_obj, 1, buf, 16);
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_write_8bit_registers (soft_spi_info_struct *soft_spi_obj, const uint8 register_name, const uint8 *data, uint32 len)
{
    soft_spi_8bit_data_handler(soft_spi_obj, register_name);
    while(len --)
    {
        soft_spi_8bit_data_handler(soft_spi_obj, *data ++);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ��� SPI �ӿ��򴫸����ļĴ���д 16bit ����
// ����˵��     *soft_spi_obj   ��� SPI ָ����Ϣ��Žṹ���ָ��
// ����˵��     register_name   �Ĵ�����ַ
// ����˵��     data            ���͵�����
// ���ز���     void
// ʹ��ʾ��     soft_spi_write_16bit_register(&soft_spi_obj, 1, 1);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_write_16bit_register (soft_spi_info_struct *soft_spi_obj, const uint16 register_name, uint16 data)
{
    soft_spi_16bit_data_handler(soft_spi_obj, register_name);
    soft_spi_16bit_data_handler(soft_spi_obj, data);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ��� SPI �ӿ��򴫸����ļĴ���д 16bit ����
// ����˵��     *soft_spi_obj   ��� SPI ָ����Ϣ��Žṹ���ָ��
// ����˵��     register_name   �Ĵ�����ַ
// ����˵��     *data           ���ݴ�Ż�����
// ����˵��     len             ����������
// ���ز���     void
// ʹ��ʾ��     soft_spi_write_16bit_registers(&soft_spi_obj, 1, buf, 16);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_write_16bit_registers (soft_spi_info_struct *soft_spi_obj, const uint16 register_name, const uint16 *data, uint32 len)
{
    soft_spi_16bit_data_handler(soft_spi_obj, register_name);
    while(len --)
    {
        soft_spi_16bit_data_handler(soft_spi_obj, *data ++);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ��� SPI �ӿڶ� 8bit ����
// ����˵��     *soft_spi_obj   ��� SPI ָ����Ϣ��Žṹ���ָ��
// ����˵��     data            ���͵�����
// ���ز���     uint8           ���ض�ȡ�� 8bit ����
// ʹ��ʾ��     soft_spi_read_8bit(&soft_spi_obj);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
uint8 soft_spi_read_8bit (soft_spi_info_struct *soft_spi_obj)
{
    return soft_spi_8bit_data_handler(soft_spi_obj, 0);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ��� SPI �ӿڶ� 8bit ����
// ����˵��     *soft_spi_obj   ��� SPI ָ����Ϣ��Žṹ���ָ��
// ����˵��     *data           ���ݴ�Ż�����
// ����˵��     len             ����������
// ���ز���     void
// ʹ��ʾ��     soft_spi_read_8bit_array(&soft_spi_obj, buf, 16);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_read_8bit_array (soft_spi_info_struct *soft_spi_obj, uint8 *data, uint32 len)
{
    while(len --)
    {
        *data ++ = soft_spi_8bit_data_handler(soft_spi_obj, 0);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ��� SPI �ӿڶ� 16bit ����
// ����˵��     *soft_spi_obj   ��� SPI ָ����Ϣ��Žṹ���ָ��
// ����˵��     data            ���͵�����
// ���ز���     uint16          ���ض�ȡ�� 16bit ����
// ʹ��ʾ��     soft_spi_read_16bit(&soft_spi_obj);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
uint16 soft_spi_read_16bit (soft_spi_info_struct *soft_spi_obj)
{
    return soft_spi_16bit_data_handler(soft_spi_obj, 0);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ��� SPI �ӿڶ� 16bit ����
// ����˵��     *soft_spi_obj   ��� SPI ָ����Ϣ��Žṹ���ָ��
// ����˵��     *data           ���ݴ�Ż�����
// ����˵��     len             ����������
// ���ز���     void
// ʹ��ʾ��     soft_spi_read_16bit_array(&soft_spi_obj, buf, 16);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_read_16bit_array (soft_spi_info_struct *soft_spi_obj, uint16 *data, uint32 len)
{
    while(len --)
    {
        *data ++ = soft_spi_16bit_data_handler(soft_spi_obj, 0);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ��� SPI �ӿڴӴ������ļĴ����� 8bit ����
// ����˵��     *soft_spi_obj   ��� SPI ָ����Ϣ��Žṹ���ָ��
// ����˵��     register_name   �Ĵ�����ַ
// ����˵��     data            ���͵�����
// ���ز���     uint8           ���ض�ȡ�� 8bit ����
// ʹ��ʾ��     soft_spi_read_8bit_register(&soft_spi_obj, 0x01, 0x01);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
uint8 soft_spi_read_8bit_register (soft_spi_info_struct *soft_spi_obj, const uint8 register_name)
{
    soft_spi_8bit_data_handler(soft_spi_obj, register_name);
    return soft_spi_8bit_data_handler(soft_spi_obj, 0);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ��� SPI �ӿڴӴ������ļĴ����� 8bit ����
// ����˵��     *soft_spi_obj   ��� SPI ָ����Ϣ��Žṹ���ָ��
// ����˵��     register_name   �Ĵ�����ַ
// ����˵��     *data           ���ݴ�Ż�����
// ����˵��     len             ����������
// ���ز���     void
// ʹ��ʾ��     soft_spi_read_8bit_registers(&soft_spi_obj, 0x01, buf, 16);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_read_8bit_registers (soft_spi_info_struct *soft_spi_obj, const uint8 register_name, uint8 *data, uint32 len)
{
    soft_spi_8bit_data_handler(soft_spi_obj, register_name);
    while(len --)
    {
        *data ++ = soft_spi_8bit_data_handler(soft_spi_obj, 0);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ��� SPI �ӿڴӴ������ļĴ����� 16bit ����
// ����˵��     *soft_spi_obj   ��� SPI ָ����Ϣ��Žṹ���ָ��
// ����˵��     register_name   �Ĵ�����ַ
// ����˵��     data            ���͵�����
// ���ز���     uint16          ���ض�ȡ�� 16bit ����
// ʹ��ʾ��     soft_spi_read_16bit_register(&soft_spi_obj, 0x0101);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
uint16 soft_spi_read_16bit_register (soft_spi_info_struct *soft_spi_obj, const uint16 register_name)
{
    soft_spi_16bit_data_handler(soft_spi_obj, register_name);
    return soft_spi_16bit_data_handler(soft_spi_obj, 0);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ��� SPI �ӿڴӴ������ļĴ����� 16bit ����
// ����˵��     *soft_spi_obj   ��� SPI ָ����Ϣ��Žṹ���ָ��
// ����˵��     register_name   �Ĵ�����ַ
// ����˵��     *data           ���ݴ�Ż�����
// ����˵��     len             ����������
// ���ز���     void
// ʹ��ʾ��     soft_spi_read_16bit_registers(&soft_spi_obj, 0x0101, buf, 16);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_read_16bit_registers (soft_spi_info_struct *soft_spi_obj, const uint16 register_name, uint16 *data, uint32 len)
{
    soft_spi_16bit_data_handler(soft_spi_obj, register_name);
    while(len --)
    {
        *data ++ = soft_spi_16bit_data_handler(soft_spi_obj, 0);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ��� SPI 8bit ���ݴ��� ���������������ͬʱ���е�
// ����˵��     *soft_spi_obj   ��� SPI ָ����Ϣ��Žṹ���ָ��
// ����˵��     write_buffer    ���͵����ݻ�������ַ
// ����˵��     read_buffer     ��������ʱ���յ������ݵĴ洢��ַ(����Ҫ������ NULL)
// ����˵��     len             ���͵��ֽ���
// ���ز���     void
// ʹ��ʾ��     soft_spi_8bit_transfer(&soft_spi_obj, buf, buf, 1);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_8bit_transfer (soft_spi_info_struct *soft_spi_obj, const uint8 *write_buffer, uint8 *read_buffer, uint32 len)
{
    while(len --)
    {
        *read_buffer = soft_spi_8bit_data_handler(soft_spi_obj, *write_buffer);
        write_buffer ++;
        read_buffer ++;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ��� SPI 16bit ���ݴ��� ���������������ͬʱ���е�
// ����˵��     *soft_spi_obj   ��� SPI ָ����Ϣ��Žṹ���ָ��
// ����˵��     write_buffer    ���͵����ݻ�������ַ
// ����˵��     read_buffer     ��������ʱ���յ������ݵĴ洢��ַ(����Ҫ������ NULL)
// ����˵��     len             ���͵��ֽ���
// ���ز���     void
// ʹ��ʾ��     soft_spi_16bit_transfer(&soft_spi_obj, buf, buf, 1);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_16bit_transfer (soft_spi_info_struct *soft_spi_obj, const uint16 *write_buffer, uint16 *read_buffer, uint32 len)
{
    while(len --)
    {
        *read_buffer = soft_spi_16bit_data_handler(soft_spi_obj, *write_buffer);
        write_buffer ++;
        read_buffer ++;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ��� SPI �ӿڳ�ʼ��
// ����˵��     *soft_spi_obj   ��� SPI ָ����Ϣ��Žṹ���ָ��
// ����˵��     mode            SPI ģʽ ���� zf_driver_spi.h �� spi_mode_enum ö���嶨��
// ����˵��     delay           ��� SPI ��ʱ ����ʱ�Ӹߵ�ƽʱ�� Խ�� SPI ����Խ��
// ����˵��     sck_pin         ѡ�� SCK ���� ���� zf_driver_gpio.h �� gpio_pin_enum ö���嶨��
// ����˵��     mosi_pin        ѡ�� MOSI ���� ���� zf_driver_gpio.h �� gpio_pin_enum ö���嶨��
// ����˵��     miso_pin        ѡ�� MISO ���� �������Ҫ������� ���� SOFT_SPI_PIN_NULL
// ����˵��     cs_pin          ѡ�� CS ���� �������Ҫ������� ���� SOFT_SPI_PIN_NULL
// ���ز���     void
// ʹ��ʾ��     spi_init(SPI_1, 0, 1*1000*1000, A5, A7, A6, A4);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_init (soft_spi_info_struct *soft_spi_obj, uint8 mode, uint32 delay, gpio_pin_enum sck_pin, gpio_pin_enum mosi_pin, uint32 miso_pin, uint32 cs_pin)
{
    zf_assert(sck_pin != mosi_pin);                                             // sck_pin  �� mosi_pin ��ô����ͬһ�����ţ�
    zf_assert(sck_pin != miso_pin);                                             // sck_pin  �� miso_pin ��ô����ͬһ�����ţ�
    zf_assert(sck_pin != cs_pin);                                               // sck_pin  �� cs_pin   ��ô����ͬһ�����ţ�
    zf_assert(mosi_pin != miso_pin);                                            // mosi_pin �� miso_pin ��ô����ͬһ�����ţ�
    zf_assert(mosi_pin != cs_pin);                                              // mosi_pin �� cs_pin   ��ô����ͬһ�����ţ�
    zf_assert((miso_pin != cs_pin) || (cs_pin == SOFT_SPI_PIN_NULL));           // miso_pin �� cs_pin   ��ô����ͬһ�����ţ�

    zf_assert(4 > mode);                                                        // ���� zf_driver_spi.h �� spi_mode_enum ö���嶨��

    soft_spi_obj->config.mode = mode;
    soft_spi_obj->delay = delay;

    soft_spi_obj->sck_pin = sck_pin;
    soft_spi_obj->mosi_pin = mosi_pin;
    if(0 == mode || 1 == mode)
    {
        gpio_init(sck_pin, GPO, GPIO_LOW, GPO_PUSH_PULL);                       // IO ��ʼ��
    }
    else
    {
        gpio_init(sck_pin, GPO, GPIO_HIGH, GPO_PUSH_PULL);                      // IO ��ʼ��
    }
    gpio_init(mosi_pin, GPO, GPIO_HIGH, GPO_PUSH_PULL);                         // IO ��ʼ��

    if(SOFT_SPI_PIN_NULL == miso_pin)
    {
        soft_spi_obj->config.use_miso = 0;
    }
    else
    {
        soft_spi_obj->config.use_miso = 1;
        soft_spi_obj->miso_pin = (gpio_pin_enum)miso_pin;
        gpio_init(soft_spi_obj->miso_pin, GPI, GPIO_HIGH, GPI_FLOATING_IN);     // IO ��ʼ��
    }
    if(SOFT_SPI_PIN_NULL == cs_pin)
    {
        soft_spi_obj->config.use_cs = 0;
    }
    else
    {
        soft_spi_obj->config.use_cs = 1;
        soft_spi_obj->cs_pin = (gpio_pin_enum)cs_pin;
        gpio_init(soft_spi_obj->cs_pin, GPO, GPIO_HIGH, GPO_PUSH_PULL);         // IO ��ʼ��
    }
}

