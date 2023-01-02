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
* �ļ�����          zf_device_mt9v03x
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
*                  TXD                �鿴 zf_device_mt9v03x.h �� MT9V03X_COF_UART_TX �궨��
*                  RXD                �鿴 zf_device_mt9v03x.h �� MT9V03X_COF_UART_RX �궨��
*                  PCLK               �鿴 zf_device_mt9v03x.h �� MT9V03X_PCLK_PIN �궨��
*                  VSY                �鿴 zf_device_mt9v03x.h �� MT9V03X_VSYNC_PIN �궨��
*                  D0-D7              �鿴 zf_device_mt9v03x.h �� MT9V03X_DATA_PIN �궨�� �Ӹö��忪ʼ�������˸�����
*                  VCC                3.3V��Դ
*                  GND                ��Դ��
*                  ������������
*                  ------------------------------------
********************************************************************************************************************/

#include "zf_common_interrupt.h"
#include "zf_common_debug.h"
#include "zf_common_fifo.h"
#include "zf_driver_soft_iic.h"
#include "zf_driver_delay.h"
#include "zf_driver_dma.h"
#include "zf_driver_exti.h"
#include "zf_driver_gpio.h"
#include "zf_driver_uart.h"
#include "zf_device_camera.h"
#include "zf_device_config.h"
#include "zf_device_mt9v03x.h"

vuint8  mt9v03x_finish_flag = 0;                             // һ��ͼ��ɼ���ɱ�־λ
IFX_ALIGN(4) uint8  mt9v03x_image[MT9V03X_H][MT9V03X_W];     // ����4�ֽڶ���

static  m9v03x_type_enum mt9v03x_type;
static  uint16    mt9v03x_version = 0x00;


int16   timeout = MT9V03X_INIT_TIMEOUT;

uint8   mt9v03x_lost_flag = 1;  // ͼ��ʧ��־λ
uint8   mt9v03x_dma_int_num;    // ��ǰDMA�жϴ���
uint8   mt9v03x_dma_init_flag;  // �Ƿ���Ҫ���³�ʼ��
uint8   mt9v03x_link_list_num;

// ��Ҫ���õ�����ͷ������ �����������޸Ĳ���
static int16 mt9v03x_set_confing_buffer[MT9V03X_CONFIG_FINISH][2]=
{
    {MT9V03X_INIT,              0},                                             // ����ͷ��ʼ��ʼ��

    {MT9V03X_AUTO_EXP,          MT9V03X_AUTO_EXP_DEF},                          // �Զ��ع�����   ��Χ1-63 0Ϊ�ر� ����Զ��ع⿪��  EXP_TIME�������õ����ݽ����Ϊ����ع�ʱ�䣬Ҳ�����Զ��ع�ʱ�������
    {MT9V03X_EXP_TIME,          MT9V03X_EXP_TIME_DEF},                          // �ع�ʱ��      ����ͷ�յ�����Զ����������ع�ʱ�䣬������ù���������Ϊ�������������ع�ֵ
    {MT9V03X_FPS,               MT9V03X_FPS_DEF},                               // ͼ��֡��      ����ͷ�յ�����Զ���������FPS���������������Ϊ������������FPS
    {MT9V03X_SET_COL,           MT9V03X_W},                                     // ͼ��������    ��Χ1-752
    {MT9V03X_SET_ROW,           MT9V03X_H},                                     // ͼ��������    ��Χ1-480
    {MT9V03X_LR_OFFSET,         MT9V03X_LR_OFFSET_DEF},                         // ͼ������ƫ����  ��ֵ ��ƫ��   ��ֵ ��ƫ��  ��Ϊ188 376 752ʱ�޷�����ƫ��    ����ͷ��ƫ�����ݺ���Զ��������ƫ�ƣ�������������ü�����������ƫ��
    {MT9V03X_UD_OFFSET,         MT9V03X_UD_OFFSET_DEF},                         // ͼ������ƫ����  ��ֵ ��ƫ��   ��ֵ ��ƫ��  ��Ϊ120 240 480ʱ�޷�����ƫ��    ����ͷ��ƫ�����ݺ���Զ��������ƫ�ƣ�������������ü�����������ƫ��
    {MT9V03X_GAIN,              MT9V03X_GAIN_DEF},                              // ͼ������      ��Χ16-64     ����������ع�ʱ��̶�������¸ı�ͼ�������̶�
    {MT9V03X_PCLK_MODE,         MT9V03X_PCLK_MODE_DEF},                         // ����ʱ��ģʽ   �������MT9V034 V2.0�Լ����ϰ汾֧�ָ�����
};

// ������ͷ�ڲ���ȡ������������ �����������޸Ĳ���
static int16 mt9v03x_get_confing_buffer[MT9V03X_CONFIG_FINISH - 1][2]=
{
    {MT9V03X_AUTO_EXP,          0},                                             // �Զ��ع�����
    {MT9V03X_EXP_TIME,          0},                                             // �ع�ʱ��
    {MT9V03X_FPS,               0},                                             // ͼ��֡��
    {MT9V03X_SET_COL,           0},                                             // ͼ��������
    {MT9V03X_SET_ROW,           0},                                             // ͼ��������
    {MT9V03X_LR_OFFSET,         0},                                             // ͼ������ƫ����
    {MT9V03X_UD_OFFSET,         0},                                             // ͼ������ƫ����
    {MT9V03X_GAIN,              0},                                             // ͼ������
    {MT9V03X_PCLK_MODE,         0},                                             // ����ʱ��ģʽ���� PCLKģʽ < ������� MT9V034 V1.5 �Լ����ϰ汾֧�ָ����� >
};

//-------------------------------------------------------------------------------------------------------------------
// �������     ��������ͷ�ڲ�������Ϣ
// ����˵��     buff            ����������Ϣ�ĵ�ַ
// ���ز���     uint8           1-ʧ�� 0-�ɹ�
// ʹ��ʾ��     mt9v03x_set_config(mt9v03x_set_confing_buffer);
// ��ע��Ϣ     �ڲ�����
//-------------------------------------------------------------------------------------------------------------------
static uint8 mt9v03x_set_config (int16 buff[MT9V03X_CONFIG_FINISH][2])
{
    uint8 return_state = 1;
    uint8  uart_buffer[4];
    uint16 temp;
    uint16 timeout_count = 0;
    uint32 loop_count = 0;
    uint32 uart_buffer_index = 0;

    switch(mt9v03x_version)
    {
        case 0x0230:    loop_count = MT9V03X_PCLK_MODE;  break;
        default:        loop_count = MT9V03X_GAIN;       break;
    }
    // ���ò���  ������ο���������ֲ�
    // ��ʼ��������ͷ�����³�ʼ��
    for(; loop_count < MT9V03X_SET_DATA; loop_count --)
    {
        uart_buffer[0] = 0xA5;
        uart_buffer[1] = (uint8)buff[loop_count][0];
        temp = buff[loop_count][1];
        uart_buffer[2] = temp >> 8;
        uart_buffer[3] = (uint8)temp;
        uart_write_buffer(MT9V03X_COF_UART, uart_buffer, 4);

        system_delay_ms(2);
    }

    do
    {
        if(3 <= fifo_used(&camera_receiver_fifo))
        {
            uart_buffer_index = 3;
            fifo_read_buffer(&camera_receiver_fifo, uart_buffer, &uart_buffer_index, FIFO_READ_AND_CLEAN);
            if((0xff == uart_buffer[1]) || (0xff == uart_buffer[2]))
            {
                return_state = 0;
                break;
            }
        }
        system_delay_ms(1);
    }while(MT9V03X_INIT_TIMEOUT > timeout_count ++);
    // ���ϲ��ֶ�����ͷ���õ�����ȫ�����ᱣ��������ͷ��51��Ƭ����eeprom��
    // ����set_exposure_time�����������õ��ع����ݲ��洢��eeprom��
    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ��ȡ����ͷ�ڲ�������Ϣ
// ����˵��     buff            ����������Ϣ�ĵ�ַ
// ���ز���     uint8           1-ʧ�� 0-�ɹ�
// ʹ��ʾ��     mt9v03x_get_config(mt9v03x_get_confing_buffer);
// ��ע��Ϣ     �ڲ�����
//-------------------------------------------------------------------------------------------------------------------
static uint8 mt9v03x_get_config (int16 buff[MT9V03X_CONFIG_FINISH - 1][2])
{
    uint8 return_state = 0;
    uint8  uart_buffer[4];
    uint16 temp;
    uint16 timeout_count = 0;
    uint32 loop_count = 0;
    uint32 uart_buffer_index = 0;

    switch(mt9v03x_version)
    {
        case 0x0230:    loop_count = MT9V03X_PCLK_MODE;  break;
        default:        loop_count = MT9V03X_GAIN;       break;
    }

    for(loop_count = loop_count - 1; loop_count >= 1; loop_count --)
    {
        if(mt9v03x_version < 0x0230 && buff[loop_count][0] == MT9V03X_PCLK_MODE)
        {
            continue;
        }
        uart_buffer[0] = 0xA5;
        uart_buffer[1] = MT9V03X_GET_STATUS;
        temp = buff[loop_count][0];
        uart_buffer[2] = temp >> 8;
        uart_buffer[3] = (uint8)temp;
        uart_write_buffer(MT9V03X_COF_UART, uart_buffer, 4);

        timeout_count = 0;
        do
        {
            if(3 <= fifo_used(&camera_receiver_fifo))
            {
                uart_buffer_index = 3;
                fifo_read_buffer(&camera_receiver_fifo, uart_buffer, &uart_buffer_index, FIFO_READ_AND_CLEAN);
                buff[loop_count][1] = uart_buffer[1] << 8 | uart_buffer[2];
                break;
            }
            system_delay_ms(1);
        }while(MT9V03X_INIT_TIMEOUT > timeout_count ++);
        if(timeout_count > MT9V03X_INIT_TIMEOUT)                                // ��ʱ
        {
            return_state = 1;
            break;
        }
    }
    return return_state;
}


//-------------------------------------------------------------------------------------------------------------------
//  �������      MT9V03X����ͷ����ͨ�Żص�
//  ����˵��      void
//  ���ز���      void
//  ʹ��ʾ��      mt9v03x_uart_callback();
//-------------------------------------------------------------------------------------------------------------------
static void mt9v03x_uart_callback (void)
{
    uint8 data = 0;
    uart_query_byte(MT9V03X_COF_UART, &data);
    if(0xA5 == data)
    {
        fifo_clear(&camera_receiver_fifo);
    }
    fifo_write_element(&camera_receiver_fifo, data);
}

//-------------------------------------------------------------------------------------------------------------------
//  �������      MT9V03X����ͷ���ж�
//  ����˵��      void
//  ���ز���      void
//  ʹ��ʾ��      mt9v03x_vsync_handler();
//-------------------------------------------------------------------------------------------------------------------
static void mt9v03x_vsync_handler(void)
{
    exti_flag_clear(MT9V03X_VSYNC_PIN);
    mt9v03x_dma_int_num = 0;
    if(mt9v03x_dma_init_flag )
    {
        mt9v03x_dma_init_flag = 0;
        IfxDma_resetChannel(&MODULE_DMA, MT9V03X_DMA_CH);
        mt9v03x_link_list_num = dma_init(MT9V03X_DMA_CH,
                                         MT9V03X_DATA_ADD,
                                         mt9v03x_image[0],
                                         MT9V03X_PCLK_PIN,
                                         EXTI_TRIGGER_RISING,
                                         MT9V03X_IMAGE_SIZE);           // �����Ƶ��300M �����ڶ�������������ΪFALLING
        dma_enable(MT9V03X_DMA_CH);
    }
    else
    {
        if(1 == mt9v03x_link_list_num)
        {
            dma_set_destination(MT9V03X_DMA_CH, mt9v03x_image[0]);      // û�в������Ӵ���ģʽ ��������Ŀ�ĵ�ַ
        }
        dma_enable(MT9V03X_DMA_CH);
    }
    mt9v03x_lost_flag = 1;
}

//-------------------------------------------------------------------------------------------------------------------
//  �������      MT9V03X����ͷDMA����ж�
//  ����˵��      void
//  ���ز���      void
//  ʹ��ʾ��      mt9v03x_dma_handler();
//-------------------------------------------------------------------------------------------------------------------
static void mt9v03x_dma_handler(void)
{
    clear_dma_flag(MT9V03X_DMA_CH);

    if(IfxDma_getChannelTransactionRequestLost(&MODULE_DMA, MT9V03X_DMA_CH)) // ͼ���λ�ж�
    {
        mt9v03x_finish_flag = 0;
        dma_disable(MT9V03X_DMA_CH);
        IfxDma_clearChannelTransactionRequestLost(&MODULE_DMA, MT9V03X_DMA_CH);
        mt9v03x_dma_init_flag = 1;
    }
    else
    {
        mt9v03x_dma_int_num++;
        if(mt9v03x_dma_int_num >= mt9v03x_link_list_num)
        {
            // �ɼ����
            // һ��ͼ��Ӳɼ���ʼ���ɼ�������ʱ3.8MS����(50FPS��188*120�ֱ���)
            mt9v03x_dma_int_num = 0;
            mt9v03x_lost_flag   = 0;
            mt9v03x_finish_flag = 1;
            dma_disable(MT9V03X_DMA_CH);
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ��ȡ����ͷ�̼��汾
// ����˵��     void
// ���ز���     uint16          0-��ȡ���� N-�汾��
// ʹ��ʾ��     mt9v03x_get_version();                          // ���øú���ǰ���ȳ�ʼ������
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
uint16 mt9v03x_get_version (void)
{
    uint16 temp;
    uint8  uart_buffer[4];
    uint16 timeout_count = 0;
    uint16 return_value = 0;
    uint32 uart_buffer_index = 0;

    uart_buffer[0] = 0xA5;
    uart_buffer[1] = MT9V03X_GET_STATUS;
    temp = MT9V03X_GET_VERSION;
    uart_buffer[2] = temp >> 8;
    uart_buffer[3] = (uint8)temp;
    uart_write_buffer(MT9V03X_COF_UART, uart_buffer, 4);

    do
    {
        if(3 <= fifo_used(&camera_receiver_fifo))
        {
            uart_buffer_index = 3;
            fifo_read_buffer(&camera_receiver_fifo, uart_buffer, &uart_buffer_index, FIFO_READ_AND_CLEAN);
            return_value = uart_buffer[1] << 8 | uart_buffer[2];
            break;
        }
        system_delay_ms(1);
    }while(MT9V03X_INIT_TIMEOUT > timeout_count ++);
    return return_value;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ������������ͷ�ع�ʱ��
// ����˵��     light           �趨�ع�ʱ��
// ���ز���     uint8           1-ʧ�� 0-�ɹ�
// ʹ��ʾ��     mt9v03x_set_exposure_time(100);                 // ���øú���ǰ���ȳ�ʼ������
// ��ע��Ϣ     �����ع�ʱ��Խ��ͼ��Խ��
//              ����ͷ�յ������ݷֱ��ʼ�FPS��������ع�ʱ��������õ����ݹ���
//              ��ô����ͷ��������������ֵ
//-------------------------------------------------------------------------------------------------------------------
uint8 mt9v03x_set_exposure_time (uint16 light)
{
    uint8 return_state = 0;
    if(MT9V03X_UART == mt9v03x_type)
    {
        uint8  uart_buffer[4];
        uint16 temp;
        uint16 timeout_count = 0;
        uint32 uart_buffer_index = 0;
        set_camera_type(CAMERA_GRAYSCALE, mt9v03x_vsync_handler, mt9v03x_dma_handler, mt9v03x_uart_callback);    // ������������ͷ����
        uart_buffer[0] = 0xA5;
        uart_buffer[1] = MT9V03X_SET_EXP_TIME;
        temp = light;
        uart_buffer[2] = temp >> 8;
        uart_buffer[3] = (uint8)temp;
        uart_write_buffer(MT9V03X_COF_UART, uart_buffer, 4);

        do
        {
            if(3 <= fifo_used(&camera_receiver_fifo))
            {
                uart_buffer_index = 3;
                fifo_read_buffer(&camera_receiver_fifo, uart_buffer, &uart_buffer_index, FIFO_READ_AND_CLEAN);
                temp = uart_buffer[1] << 8 | uart_buffer[2];
                break;
            }
            system_delay_ms(1);
        }while(MT9V03X_INIT_TIMEOUT > timeout_count ++);
        if((temp != light) || (MT9V03X_INIT_TIMEOUT <= timeout_count))
        {
            return_state = 1;
        }
    }
    else
    {
        return_state = mt9v03x_set_exposure_time_sccb(light);
    }
    set_camera_type(CAMERA_GRAYSCALE, mt9v03x_vsync_handler, mt9v03x_dma_handler, NULL);    // ������������ͷ����
    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ������ͷ�ڲ��Ĵ�������д����
// ����˵��     addr            ����ͷ�ڲ��Ĵ�����ַ
// ����˵��     data            ��Ҫд�������
// ���ز���     uint8           1-ʧ�� 0-�ɹ�
// ʹ��ʾ��     mt9v03x_set_reg(addr, data);                    // ���øú���ǰ���ȳ�ʼ������
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
uint8 mt9v03x_set_reg (uint8 addr, uint16 data)
{
    uint8 return_state = 0;
    if(MT9V03X_UART == mt9v03x_type)
    {
        uint8  uart_buffer[4];
        uint16 temp;
        uint16 timeout_count = 0;
        uint32 uart_buffer_index = 0;
        set_camera_type(CAMERA_GRAYSCALE, mt9v03x_vsync_handler, mt9v03x_dma_handler, mt9v03x_uart_callback);    // ������������ͷ����
        uart_buffer[0] = 0xA5;
        uart_buffer[1] = MT9V03X_SET_ADDR;
        temp = addr;
        uart_buffer[2] = temp >> 8;
        uart_buffer[3] = (uint8)temp;
        uart_write_buffer(MT9V03X_COF_UART, uart_buffer, 4);

        system_delay_ms(10);
        uart_buffer[0] = 0xA5;
        uart_buffer[1] = MT9V03X_SET_DATA;
        temp = data;
        uart_buffer[2] = temp >> 8;
        uart_buffer[3] = (uint8)temp;
        uart_write_buffer(MT9V03X_COF_UART, uart_buffer, 4);

        do
        {
            if(3 <= fifo_used(&camera_receiver_fifo))
            {
                uart_buffer_index = 3;
                fifo_read_buffer(&camera_receiver_fifo, uart_buffer, &uart_buffer_index, FIFO_READ_AND_CLEAN);
                temp = uart_buffer[1] << 8 | uart_buffer[2];
                break;
            }
            system_delay_ms(1);
        }while(MT9V03X_INIT_TIMEOUT > timeout_count ++);
        if((temp != data) || (MT9V03X_INIT_TIMEOUT <= timeout_count))
        {
            return_state = 1;
        }
    }
    else
    {
        return_state = mt9v03x_set_reg_sccb(addr, data);
    }
    set_camera_type(CAMERA_GRAYSCALE, mt9v03x_vsync_handler, mt9v03x_dma_handler, NULL);    // ������������ͷ����
    return return_state;
}


//-------------------------------------------------------------------------------------------------------------------
// �������     MT9V03X ����ͷ��ʼ��
// ����˵��     void
// ���ز���     uint8           1-ʧ�� 0-�ɹ�
// ʹ��ʾ��     zf_log(mt9v03x_init(), "mt9v03x init error");
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
uint8 mt9v03x_init (void)
{
    uint8 return_state = 0;
    soft_iic_info_struct mt9v03x_iic_struct;
    do
    {
        system_delay_ms(500);
        set_camera_type(CAMERA_GRAYSCALE, NULL, NULL, NULL);    // ������������ͷ����
        // ���ȳ���SCCBͨѶ
        mt9v03x_type = MT9V03X_SCCB;
        soft_iic_init(&mt9v03x_iic_struct, 0, MT9V03X_COF_IIC_DELAY, MT9V03X_COF_IIC_SCL, MT9V03X_COF_IIC_SDA);
        if(mt9v03x_set_config_sccb(&mt9v03x_iic_struct, mt9v03x_set_confing_buffer))
        {
            // SCCBͨѶʧ�ܣ����Դ���ͨѶ
            mt9v03x_type = MT9V03X_UART;
            camera_fifo_init();
            set_camera_type(CAMERA_GRAYSCALE, NULL, NULL, mt9v03x_uart_callback);    // ������������ͷ����
            uart_init (MT9V03X_COF_UART, MT9V03X_COF_BAUR, MT9V03X_COF_UART_RX, MT9V03X_COF_UART_TX);   //��ʼ������ ��������ͷ
            uart_rx_interrupt(MT9V03X_COF_UART, 1);
            fifo_clear(&camera_receiver_fifo);
            mt9v03x_version = mt9v03x_get_version();                                // ��ȡ���õķ�ʽ

            if(mt9v03x_set_config(mt9v03x_set_confing_buffer))
            {
                // �������������˶�����Ϣ ������ʾ����λ��������
                // ��ô����ͨ�ų�����ʱ�˳���
                // ���һ�½�����û������ ���û������ܾ��ǻ���
                zf_log(0, "MT9V03X set config error.");
                set_camera_type(NO_CAMERE, NULL, NULL, NULL);
                return_state = 1;
                break;
            }

            // ��ȡ���ñ��ڲ鿴�����Ƿ���ȷ
            if(mt9v03x_get_config(mt9v03x_get_confing_buffer))
            {
                // �������������˶�����Ϣ ������ʾ����λ��������
                // ��ô���Ǵ���ͨ�ų�����ʱ�˳���
                // ���һ�½�����û������ ���û������ܾ��ǻ���
                zf_log(0, "MT9V03X get config error.");
                set_camera_type(NO_CAMERE, NULL, NULL, NULL);
                return_state = 1;
                break;
            }
        }
        set_camera_type(CAMERA_GRAYSCALE, mt9v03x_vsync_handler, mt9v03x_dma_handler, NULL);    // ������������ͷ����
        mt9v03x_link_list_num = camera_init(MT9V03X_DATA_ADD, mt9v03x_image[0], MT9V03X_IMAGE_SIZE);
    }while(0);

    return return_state;
}

