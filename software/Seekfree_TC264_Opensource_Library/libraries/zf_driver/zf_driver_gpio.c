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
* �ļ�����          zf_driver_gpio
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

#include "zf_driver_gpio.h"

//-------------------------------------------------------------------------------------------------------------------
//  �������      ��ȡGPIO����ַ
//  ����˵��      pin         ѡ������� (��ѡ��Χ�� zf_driver_gpio.h ��gpio_pin_enumö��ֵȷ��)
//  ���ز���      void
//  ʹ��ʾ��      get_port(P00_0)
//  ��ע��Ϣ      ���������ļ��ڲ����� �û����ù�ע Ҳ�����޸�
//-------------------------------------------------------------------------------------------------------------------
Ifx_P* get_port (gpio_pin_enum pin)
{
    volatile Ifx_P *port;

    switch(pin&0xffe0)
    {
        case P00_0: port = &MODULE_P00; break;
        case P02_0: port = &MODULE_P02; break;
        case P10_0: port = &MODULE_P10; break;
        case P11_0: port = &MODULE_P11; break;
        case P13_0: port = &MODULE_P13; break;
        case P14_0: port = &MODULE_P14; break;
        case P15_0: port = &MODULE_P15; break;
        case P20_0: port = &MODULE_P20; break;
        case P21_0: port = &MODULE_P21; break;
        case P22_0: port = &MODULE_P22; break;
        case P23_0: port = &MODULE_P23; break;
        case P32_0: port = &MODULE_P32; break;
        case P33_0: port = &MODULE_P33; break;
        default:break;
    }
#pragma warning 507
    return port;
#pragma warning default
}

//-------------------------------------------------------------------------------------------------------------------
// �������     gpio �������
// ����˵��     pin         ѡ������� (��ѡ��Χ�� zf_driver_gpio.h �� gpio_pin_enum ö��ֵȷ��)
// ����˵��     dat         0���͵�ƽ 1���ߵ�ƽ
// ���ز���     void
// ʹ��ʾ��     gpio_set_level(P00_0, 1);// P00_0 ����ߵ�ƽ
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void gpio_set_level (gpio_pin_enum pin, uint8 dat)
{
    if(dat)
    {
        IfxPort_setPinHigh(get_port(pin), pin&0x1f);
    }
    else
    {
        IfxPort_setPinLow(get_port(pin), pin&0x1f);
    }
}
//-------------------------------------------------------------------------------------------------------------------
// �������     gpio ��ƽ��ȡ
// ����˵��     pin         ѡ������� (��ѡ��Χ�� zf_driver_gpio.h �� gpio_pin_enum ö��ֵȷ��)
// ���ز���     uint8       ���ŵ�ǰ��ƽ
// ʹ��ʾ��     uint8 status = gpio_get_level(P00_0);// ��ȡP00_0���ŵ�ƽ
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
uint8 gpio_get_level (gpio_pin_enum pin)
{
    return IfxPort_getPinState(get_port(pin), pin&0x1f);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     gpio ��ת��ƽ
// ����˵��     pin         ѡ������� (��ѡ��Χ�� zf_driver_gpio.h �� gpio_pin_enum ö��ֵȷ��)
// ���ز���     void
// ʹ��ʾ��     gpio_toggle_level(P00_0);// P00_0���ŵ�ƽ��ת
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void gpio_toggle_level (gpio_pin_enum pin)
{
    IfxPort_togglePin(get_port(pin), pin&0x1f);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     gpio ��������
// ����˵��     pin         ѡ������� (��ѡ��Χ�� zf_driver_gpio.h �� gpio_pin_enum ö��ֵȷ��)
// ����˵��     dir         ���ŵķ���   �����GPO   ���룺GPI
// ����˵��     mode        ���ŵ�ģʽ (��ѡ��Χ�� zf_driver_gpio.h �� gpio_mode_enum ö��ֵȷ��)
// ���ز���     void
// ʹ��ʾ��     gpio_dir(P00_0, GPO, GPO_PUSH_PULL);// ����P00_0Ϊ�������ģʽ
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void gpio_set_dir (gpio_pin_enum pin, gpio_dir_enum dir, gpio_mode_enum pinmode)
{
    IfxPort_Mode port_mode;
    if(dir == GPI)
    {
        switch(pinmode)
        {
            case GPI_FLOATING_IN:port_mode = IfxPort_Mode_inputNoPullDevice ;       break; // ��������
            case GPI_PULL_DOWN  :port_mode = IfxPort_Mode_inputPullDown     ;       break; // ��������
            default:             port_mode = IfxPort_Mode_inputPullUp       ;       break; // Ĭ��Ϊ��������
        }
    }
    else
    {
        switch(pinmode)
        {
            case GPO_OPEN_DTAIN :port_mode = IfxPort_Mode_outputOpenDrainGeneral;   break;// ��©���
            default:             port_mode = IfxPort_Mode_outputPushPullGeneral ;   break;// Ĭ��Ϊ�������
        }
    }

    IfxPort_setPinMode(get_port(pin), pin&0x1f, port_mode);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     gpio ��ʼ��
// ����˵��     pin         ѡ������� (��ѡ��Χ�� zf_driver_gpio.h �� gpio_pin_enum ö��ֵȷ��)
// ����˵��     mode        ���ŵķ��� [GPI/GPIO]
// ����˵��     dat         ���ų�ʼ��ʱ���õĵ�ƽ״̬�����ʱ��Ч 0���͵�ƽ 1���ߵ�ƽ ��������Ϊ���ģʽʱ��Ч
// ����˵��     mode        ���ŵ�ģʽ (��ѡ��Χ�� zf_driver_gpio.h �� gpio_mode_enum ö��ֵȷ��)
// ���ز���     void
// ʹ��ʾ��     gpio_init(P00_0, GPO, 1, GPO_PUSH_PULL); // P00_0��ʼ��ΪGPIO���ܡ����ģʽ������ߵ�ƽ���������
// ��ע��Ϣ     ��Ҫ�ر�ע��P20_2�ǲ�����������ģ�����ֻ������Ĺ���
//-------------------------------------------------------------------------------------------------------------------
void gpio_init (gpio_pin_enum pin, gpio_dir_enum dir, uint8 dat, gpio_mode_enum pinmode)
{
    IfxPort_Mode port_mode;

    if(dir == GPI)
    {
        switch(pinmode)
        {
            case GPI_FLOATING_IN:port_mode = IfxPort_Mode_inputNoPullDevice ;       break; // ��������
            case GPI_PULL_DOWN  :port_mode = IfxPort_Mode_inputPullDown     ;       break; // ��������
            default:             port_mode = IfxPort_Mode_inputPullUp       ;       break; // Ĭ��Ϊ��������
        }
    }
    else
    {
        switch(pinmode)
        {
            case GPO_OPEN_DTAIN :port_mode = IfxPort_Mode_outputOpenDrainGeneral;   break;// ��©���
            default:             port_mode = IfxPort_Mode_outputPushPullGeneral ;   break;// Ĭ��Ϊ�������
        }
    }

    IfxPort_setPinMode(get_port(pin), pin&0x1f, port_mode);

    IfxPort_setPinPadDriver(get_port(pin), pin&0x1f, IfxPort_PadDriver_cmosAutomotiveSpeed1);

    if(dir == GPO)
    {
        if(dat)
        {
            IfxPort_setPinHigh(get_port(pin), pin&0x1f);
        }
        else
        {
            IfxPort_setPinLow(get_port(pin), pin&0x1f);
        }
    }

}
