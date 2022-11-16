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
* �ļ�����          zf_driver_pit
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

#include "IfxCcu6_Timer.h"
#include "SysSe/Bsp/Bsp.h"
#include "isr_config.h"
#include "zf_driver_pit.h"

//-------------------------------------------------------------------------------------------------------------------
//  �������      pit�ر�
//  ����˵��      pit_index        ѡ��CCU6ģ��
//  ���ز���      void
//  ʹ��ʾ��      pit_close(CCU60_CH0); // �ر�CCU60 ͨ��0�ļ�ʱ��
//  ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void pit_close (pit_index_enum pit_index)
{
    volatile Ifx_CCU6 *module;
    IfxCcu6_Timer g_Ccu6Timer;

    module = IfxCcu6_getAddress((IfxCcu6_Index)(pit_index / 2));

    g_Ccu6Timer.ccu6 = module;
    g_Ccu6Timer.timer = (IfxCcu6_TimerId)((pit_index % 2));

    IfxCcu6_Timer_stop(&g_Ccu6Timer);
}

//-------------------------------------------------------------------------------------------------------------------
//  �������      pit����
//  ����˵��      pit_index           ѡ��CCU6ģ��
//  ���ز���      void
//  ʹ��ʾ��      pit_start(CCU60_CH0); // ��CCU60 ͨ��0�ļ�ʱ��
//  ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void pit_start (pit_index_enum pit_index)
{
    volatile Ifx_CCU6 *module;
    IfxCcu6_Timer g_Ccu6Timer;

    module = IfxCcu6_getAddress((IfxCcu6_Index)(pit_index / 2));

    g_Ccu6Timer.ccu6 = module;
    g_Ccu6Timer.timer = (IfxCcu6_TimerId)((pit_index % 2));

    IfxCcu6_Timer_start(&g_Ccu6Timer);
}

//-------------------------------------------------------------------------------------------------------------------
//  �������      ��ֹ����pit�ж�
//  ���ز���      void
//  ʹ��ʾ��      pit_interrupt_all_close();
//  ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void pit_all_close (void)
{
    IfxCcu6_disableModule((Ifx_CCU6 *)IfxCcu6_cfg_indexMap[0].module);
    IfxCcu6_disableModule((Ifx_CCU6 *)IfxCcu6_cfg_indexMap[1].module);
}


//-------------------------------------------------------------------------------------------------------------------
//  �������      ʧ��pit�ж�
//  ����˵��      pit_index           ѡ��CCU6ģ��
//  ���ز���      void
//  ʹ��ʾ��      pit_disable_interrupt(CCU60_CH0); // ��ֹCCU60 ͨ��0���ж�
//  ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void pit_disable (pit_index_enum pit_index)
{
    volatile Ifx_CCU6 *module;
    module = IfxCcu6_getAddress((IfxCcu6_Index)(pit_index / 2));
    IfxCcu6_disableInterrupt(module, (pit_index % 2) * 2 + 7);
}

//-------------------------------------------------------------------------------------------------------------------
//  �������      ʹ��pit�ж�
//  ����˵��      pit_index           ѡ��CCU6ģ��
//  ���ز���      void
//  ʹ��ʾ��      pit_enable_interrupt(CCU60_CH0);  // ����CCU60 ͨ��0���ж�
//  ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void pit_enable (pit_index_enum pit_index)
{
    volatile Ifx_CCU6 *module;
    module = IfxCcu6_getAddress((IfxCcu6_Index)(pit_index / 2));
    IfxCcu6_enableInterrupt(module, (pit_index % 2) * 2 + 7);
}

//-------------------------------------------------------------------------------------------------------------------
//  �������      pit��ʼ��
//  ����˵��      pit_index           ѡ��CCU6ģ��
//  ����˵��      time                ����ʱ��
//  ���ز���      void
//  ʹ��ʾ��      pit_init(CCU60_CH0, 5000);      // ���������ж�5000us
//  ��ע��Ϣ      ��ʹ��.h�ļ��� ��ʱ�䵥λ�ĺ궨�庯��
//-------------------------------------------------------------------------------------------------------------------
void pit_init (pit_index_enum pit_index, uint32 time)
{
    uint8 i;
    volatile Ifx_CCU6 *module;
    uint64 timer_input_clk;
    IfxCcu6_Timer g_Ccu6Timer;
    IfxCcu6_Timer_Config timerConfig;
    uint32 timer_period;

    boolean interrupt_state = disableInterrupts();

    module = IfxCcu6_getAddress((IfxCcu6_Index)(pit_index/2));

    IfxCcu6_Timer_initModuleConfig(&timerConfig, module);



    timer_input_clk = IfxScuCcu_getSpbFrequency();
    i = 0;
    while(i < 16)
    {
        timer_period = (uint32)(timer_input_clk * time / 1000000);
        if(timer_period < 0xffff)   break;
        timer_input_clk >>= 1;
        i++;
    }
    if(16 <= i) IFX_ASSERT(IFX_VERBOSE_LEVEL_ERROR, FALSE);


    switch(pit_index)
    {
        case CCU60_CH0:
        {
            timerConfig.interrupt1.typeOfService  = CCU6_0_CH0_INT_SERVICE;
            timerConfig.interrupt1.priority       = CCU6_0_CH0_ISR_PRIORITY;
            break;
        }
        case CCU60_CH1:
        {
            timerConfig.interrupt2.typeOfService  = CCU6_0_CH1_INT_SERVICE;
            timerConfig.interrupt2.priority       = CCU6_0_CH1_ISR_PRIORITY;
            break;
        }
        case CCU61_CH0:
        {
            timerConfig.interrupt1.typeOfService  = CCU6_1_CH0_INT_SERVICE;
            timerConfig.interrupt1.priority       = CCU6_1_CH0_ISR_PRIORITY;
            break;
        }
        case CCU61_CH1:
        {
            timerConfig.interrupt2.typeOfService  = CCU6_1_CH1_INT_SERVICE;
            timerConfig.interrupt2.priority       = CCU6_1_CH1_ISR_PRIORITY;
            break;
        }
    }

    if((pit_index % 2) == 0)
    {
        timerConfig.timer = IfxCcu6_TimerId_t12;
        timerConfig.interrupt1.source          = IfxCcu6_InterruptSource_t12PeriodMatch;
        timerConfig.interrupt1.serviceRequest  = IfxCcu6_ServiceRequest_1;
        timerConfig.base.t12Period             = timer_period;
        timerConfig.base.t12Frequency          = (float)timer_input_clk;
        timerConfig.clock.t12countingInputMode = IfxCcu6_CountingInputMode_internal;
    }
    else
    {
        timerConfig.timer = IfxCcu6_TimerId_t13;
        timerConfig.interrupt2.source          = IfxCcu6_InterruptSource_t13PeriodMatch;
        timerConfig.interrupt2.serviceRequest  = IfxCcu6_ServiceRequest_2;
        timerConfig.base.t13Period             = timer_period;
        timerConfig.base.t13Frequency          = (float)timer_input_clk;
        timerConfig.clock.t13countingInputMode = IfxCcu6_CountingInputMode_internal;
    }
    timerConfig.timer12.counterValue     = 0;
    timerConfig.timer13.counterValue     = 0;
    timerConfig.trigger.t13InSyncWithT12 = FALSE;

    IfxCcu6_Timer_initModule(&g_Ccu6Timer, &timerConfig);

    restoreInterrupts(interrupt_state);

    IfxCcu6_setSuspendMode(module, IfxCcu6_SuspendMode_hard);
    IfxCcu6_Timer_start(&g_Ccu6Timer);
}
