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
* �ļ�����          zf_driver_exti
* ��˾����          �ɶ���ɿƼ����޹�˾
* �汾��Ϣ          �鿴 libraries/doc �ļ����� version �ļ� �汾˵��
* ��������          ADS v1.8.0
* ����ƽ̨          TC264D
* ��������          https://seekfree.taobao.com/
*
* �޸ļ�¼
* ����              ����                ��ע
* 2022-09-15       pudding             first version
********************************************************************************************************************/

#include "Src/Std/IfxSrc.h"
#include "SysSe/Bsp/Bsp.h"
#include "isr_config.h"
#include "zf_common_debug.h"
#include "zf_driver_exti.h"

//-------------------------------------------------------------------------------------------------------------------
//  �������     ��ȡ�˿ڲ���
//  ���ز���     IfxScu_Req_In
//  ʹ��ʾ��     get_exit_pin(ERU_CH0_REQ4_P10_7);
//  ��ע��Ϣ     �ڲ�����
//-------------------------------------------------------------------------------------------------------------------
static IfxScu_Req_In* get_exit_pin (exti_pin_enum exti_pin)
{
    IfxScu_Req_In* get_exit_pin_config;

    switch(exti_pin)
    {
        case ERU_CH0_REQ0_P15_4:   get_exit_pin_config =  &IfxScu_REQ0_P15_4_IN;   break;
        case ERU_CH1_REQ10_P14_3:  get_exit_pin_config =  &IfxScu_REQ10_P14_3_IN;  break;
        case ERU_CH2_REQ7_P00_4:   get_exit_pin_config =  &IfxScu_REQ7_P00_4_IN;   break;
        case ERU_CH2_REQ14_P02_1:  get_exit_pin_config =  &IfxScu_REQ14_P02_1_IN;  break;
        case ERU_CH2_REQ2_P10_2:   get_exit_pin_config =  &IfxScu_REQ2_P10_2_IN;   break;
        case ERU_CH3_REQ6_P02_0:   get_exit_pin_config =  &IfxScu_REQ6_P02_0_IN;   break;
        case ERU_CH3_REQ3_P10_3:   get_exit_pin_config =  &IfxScu_REQ3_P10_3_IN;   break;
        case ERU_CH3_REQ15_P14_1:  get_exit_pin_config =  &IfxScu_REQ15_P14_1_IN;  break;
        case ERU_CH4_REQ13_P15_5:  get_exit_pin_config =  &IfxScu_REQ13_P15_5_IN;  break;
        case ERU_CH4_REQ8_P33_7:   get_exit_pin_config =  &IfxScu_REQ8_P33_7_IN;   break;
        case ERU_CH5_REQ1_P15_8:   get_exit_pin_config =  &IfxScu_REQ1_P15_8_IN;   break;
        case ERU_CH6_REQ12_P11_10: get_exit_pin_config =  &IfxScu_REQ12_P11_10_IN; break;
        case ERU_CH6_REQ9_P20_0:   get_exit_pin_config =  &IfxScu_REQ9_P20_0_IN;   break;
        case ERU_CH7_REQ16_P15_1:  get_exit_pin_config =  &IfxScu_REQ16_P15_1_IN;  break;
        case ERU_CH7_REQ11_P20_9:  get_exit_pin_config =  &IfxScu_REQ11_P20_9_IN;  break;
        default: zf_assert(FALSE); get_exit_pin_config =  NULL;
    }

    return get_exit_pin_config;
}



//-------------------------------------------------------------------------------------------------------------------
// �������     EXTI �ж�ʹ��
// ����˵��     pin             ѡ�� EXTI ���� (��ѡ��Χ�� zf_driver_exti.h �� exti_pin_enum ö��ֵȷ��)
// ���ز���     void
// ʹ��ʾ��     exti_enable(ERU_CH0_REQ4_P10_7);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void exti_enable (exti_pin_enum eru_pin)
{
    IfxScuEru_OutputChannel    outputChannel = (IfxScuEru_OutputChannel)(eru_pin/3);

    volatile Ifx_SRC_SRCR *src = &MODULE_SRC.SCU.SCU.ERU[(int)outputChannel % 4];
    IfxSrc_enable(src);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     EXTI �ж�ʧ��
// ����˵��     pin             ѡ�� EXTI ���� (��ѡ��Χ�� zf_driver_exti.h �� exti_pin_enum ö��ֵȷ��)
// ���ز���     void
// ʹ��ʾ��     exti_disable(A0);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void exti_disable (exti_pin_enum eru_pin)
{
    IfxScuEru_OutputChannel    outputChannel = (IfxScuEru_OutputChannel)(eru_pin/3);

    volatile Ifx_SRC_SRCR *src = &MODULE_SRC.SCU.SCU.ERU[(int)outputChannel % 4];
    IfxSrc_disable(src);
}

//-------------------------------------------------------------------------------------------------------------------
//  �������      EXTI ʧ��
//  ���ز���      void
//  ʹ��ʾ��      exti_disable(ERU_CH0_REQ0_P15_4);
//-------------------------------------------------------------------------------------------------------------------
void exti_all_close (void)
{
    volatile Ifx_SRC_SRCR *src;
    int8 channel;
    for(channel = 0; channel < 4; channel ++)
    {
        src = &MODULE_SRC.SCU.SCU.ERU[channel];
        IfxSrc_deinit(src);
    }
}
//-------------------------------------------------------------------------------------------------------------------
//  �������      EXTI �жϳ�ʼ��
//  ����˵��      eru_pin         ����eruͨ��������
//  ����˵��      trigger         ���ô�����ʽ
//  ���ز���      void
//  ʹ��ʾ��      exti_init(ERU_CH0_REQ0_P15_4, EXTI_TRIGGER_RISING);   // eruͨ��0 ʹ��P15_4���ţ������ش����ж�
//  ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void exti_init (exti_pin_enum exti_pin, exti_trigger_enum trigger)
{
    boolean interrupt_state = disableInterrupts();

    IfxScu_Req_In *reqPin;

    reqPin = get_exit_pin(exti_pin);

    IfxScuEru_initReqPin(reqPin, IfxPort_InputMode_pullUp);

    IfxScuEru_InputChannel inputChannel = (IfxScuEru_InputChannel)reqPin->channelId;

    IfxScuEru_InputNodePointer triggerSelect = (IfxScuEru_InputNodePointer)(exti_pin/3);
    IfxScuEru_OutputChannel    outputChannel = (IfxScuEru_OutputChannel)(exti_pin/3);

    switch(trigger)
    {
        case EXTI_TRIGGER_RISING:
        {
            IfxScuEru_disableFallingEdgeDetection(inputChannel);
            IfxScuEru_enableRisingEdgeDetection(inputChannel);
        }break;

        case EXTI_TRIGGER_FALLING:
        {
            IfxScuEru_enableFallingEdgeDetection(inputChannel);
            IfxScuEru_disableRisingEdgeDetection(inputChannel);
        }break;

        case EXTI_TRIGGER_BOTH:
        {
            IfxScuEru_enableFallingEdgeDetection(inputChannel);
            IfxScuEru_enableRisingEdgeDetection(inputChannel);
        }break;

        default: zf_assert(FALSE);
    }

    IfxScuEru_enableTriggerPulse(inputChannel);
    IfxScuEru_connectTrigger(inputChannel, triggerSelect);

    IfxScuEru_setFlagPatternDetection(outputChannel, inputChannel, TRUE);
    IfxScuEru_enablePatternDetectionTrigger(outputChannel);
    IfxScuEru_setInterruptGatingPattern(outputChannel, IfxScuEru_InterruptGatingPattern_alwaysActive);


    volatile Ifx_SRC_SRCR *src = &MODULE_SRC.SCU.SCU.ERU[(int)outputChannel % 4];
    IfxSrc_Tos exit_service;
    uint8 exit_priority;
    switch((exti_pin/3)%4)
    {
        case 0:
        {
            exit_service  = EXTI_CH0_CH4_INT_SERVICE;
            exit_priority = EXTI_CH0_CH4_INT_PRIO;
        }break;

        case 1:
        {
            exit_service  = EXTI_CH1_CH5_INT_SERVICE;
            exit_priority = EXTI_CH1_CH5_INT_PRIO;
        }break;

        case 2:
        {
            exit_service  = EXTI_CH2_CH6_INT_SERVICE;
            exit_priority = EXTI_CH2_CH6_INT_PRIO;
        }break;

        case 3:
        {
            exit_service  = EXTI_CH3_CH7_INT_SERVICE;
            exit_priority = EXTI_CH3_CH7_INT_PRIO;
        }break;

    }
#pragma warning 507
    IfxSrc_init(src, exit_service, exit_priority);
#pragma warning default
    IfxSrc_enable(src);

    restoreInterrupts(interrupt_state);
}
