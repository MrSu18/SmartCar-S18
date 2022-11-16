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
* �ļ�����          zf_driver_dma
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

#include "IfxDma_Dma.h"
#include "IfxScuEru.h"
#include "isr_config.h"
#include "zf_common_debug.h"
#include "zf_driver_dma.h"

typedef struct
{
    Ifx_DMA_CH linked_list[8];              // DMA����
    IfxDma_Dma_Channel channel;             // DMAͨ�����
}DMA_LINK;

#if(0 == DMA_INT_SERVICE)
#pragma section all "cpu0_dsram"
IFX_ALIGN(256) DMA_LINK dma_link_list;

#elif(1 == DMA_INT_SERVICE)
#pragma section all "cpu1_dsram"
IFX_ALIGN(256) DMA_LINK dma_link_list;

#endif
#pragma section all restore
//-------------------------------------------------------------------------------------------------------------------
// �������      dma��ʼ��
// ����˵��      dma_ch              ѡ��DMAͨ��
// ����˵��      source_addr         ����Դ��ַ
// ����˵��      destination_addr    ����Ŀ�ĵ�ַ
// ����˵��      exti_pin            ���ô�����eruͨ��
// ����˵��      trigger             ���ô�����ʽ
// ����˵��      dma_count           ����dma���ƴ���
// ���ز���      uint8
// ʹ��ʾ��      dma_init(MT9V03X_DMA_CH, MT9V03X_DATA_ADD, mt9v03x_image[0], MT9V03X_PCLK_PIN, EXTI_TRIGGER_RISING, MT9V03X_IMAGE_SIZE);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
uint8 dma_init (IfxDma_ChannelId dma_ch, uint8 *source_addr, uint8 *destination_addr, exti_pin_enum exti_pin, exti_trigger_enum trigger, uint16 dma_count)
{
    IfxDma_Dma_Channel dmaChn;

    exti_init(exti_pin, trigger);               // eru����DMAͨ����   ��eru�ļ�������eru�����ȼ�����Ϊ������ͨ��

    IfxDma_Dma_Config        dmaConfig;
    IfxDma_Dma_initModuleConfig(&dmaConfig, &MODULE_DMA);

    IfxDma_Dma               dma;
    IfxDma_Dma_initModule(&dma, &dmaConfig);

    IfxDma_Dma_ChannelConfig cfg;
    IfxDma_Dma_initChannelConfig(&cfg, &dma);

    uint8  list_num, i;
    uint16 single_channel_dma_count;

    zf_assert(!(dma_count%8));                  // �����������Ϊ8�ı���


    list_num = 1;
    single_channel_dma_count = dma_count / list_num;
    if(16384 < single_channel_dma_count)
    {
        while(TRUE)
        {
            single_channel_dma_count = dma_count / list_num;
            if((single_channel_dma_count <= 16384) && !(dma_count % list_num))
            {
                break;
            }
            list_num++;
            if(list_num > 8)
            {
                zf_assert(FALSE);
            }
        }
    }


    if(1 == list_num)
    {
        cfg.shadowControl               = IfxDma_ChannelShadow_none;
        cfg.operationMode               = IfxDma_ChannelOperationMode_single;
        cfg.shadowAddress               = 0;
    }
    else
    {
        cfg.shadowControl               = IfxDma_ChannelShadow_linkedList;
        cfg.operationMode               = IfxDma_ChannelOperationMode_continuous;
        cfg.shadowAddress               = IFXCPU_GLB_ADDR_DSPR(IfxCpu_getCoreId(), (unsigned)&dma_link_list.linked_list[1]);
    }

    cfg.requestMode                     = IfxDma_ChannelRequestMode_oneTransferPerRequest;
    cfg.moveSize                        = IfxDma_ChannelMoveSize_8bit;
    cfg.busPriority                     = IfxDma_ChannelBusPriority_high;

    cfg.sourceAddress                   = IFXCPU_GLB_ADDR_DSPR(IfxCpu_getCoreId(), source_addr);
    cfg.sourceAddressCircularRange      = IfxDma_ChannelIncrementCircular_none;
    cfg.sourceCircularBufferEnabled     = TRUE;

    cfg.destinationAddressIncrementStep = IfxDma_ChannelIncrementStep_1;

    cfg.channelId                       = (IfxDma_ChannelId)dma_ch;
    cfg.hardwareRequestEnabled          = FALSE;
    cfg.channelInterruptEnabled         = TRUE;
    cfg.channelInterruptPriority        = DMA_INT_PRIO;
    cfg.channelInterruptTypeOfService   = DMA_INT_SERVICE;



    cfg.destinationAddress              = IFXCPU_GLB_ADDR_DSPR(IfxCpu_getCoreId(), destination_addr);

    cfg.transferCount                   = single_channel_dma_count;

    IfxDma_Dma_initChannel(&dmaChn, &cfg);

    if(1 < list_num)
    {
        i = 0;
        while(i < list_num)
        {
            cfg.destinationAddress      = IFXCPU_GLB_ADDR_DSPR(IfxCpu_getCoreId(), destination_addr + single_channel_dma_count * i);
            if(i == (list_num - 1))
            {
                cfg.shadowAddress       = IFXCPU_GLB_ADDR_DSPR(IfxCpu_getCoreId(), (unsigned)&dma_link_list.linked_list[0]);
            }
            else
            {
                cfg.shadowAddress       = IFXCPU_GLB_ADDR_DSPR(IfxCpu_getCoreId(), (unsigned)&dma_link_list.linked_list[i+1]);
            }
            cfg.transferCount           = single_channel_dma_count;

            IfxDma_Dma_initLinkedListEntry((void *)&dma_link_list.linked_list[i], &cfg);
            i++;
        }
    }

    IfxDma_Dma_getSrcPointer(&dma_link_list.channel)->B.CLRR = 1;

    return list_num;
}


//-------------------------------------------------------------------------------------------------------------------
// �������     dma �����ֹ
// ����˵��     ch              ѡ�� dma ͨ�� (��� zf_driver_dma.h ��ö�� dma_channel_enum ����)
// ���ز���     void
// ʹ��ʾ��     dma_disable(MT9V03X_DMA_CH);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void dma_disable (IfxDma_ChannelId dma_ch)
{
    IfxDma_disableChannelTransaction(&MODULE_DMA, dma_ch);
}


//-------------------------------------------------------------------------------------------------------------------
// �������     dma ����ʹ��
// ����˵��     ch              ѡ�� dma ͨ�� (��� zf_driver_dma.h ��ö�� dma_channel_enum ����)
// ���ز���     void
// ʹ��ʾ��     dma_enable(MT9V03X_DMA_CH);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void dma_enable (IfxDma_ChannelId dma_ch)
{
    IfxDma_enableChannelTransaction(&MODULE_DMA, dma_ch);
}
