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
* �ļ�����          zf_common_clock
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

#include "IfxScuEru.h"
#include "Ifxstm.h"
#include "Cpu0_Main.h"
#include "Cpu/Std/IfxCpu.h"
#include "zf_common_interrupt.h"
#include "zf_common_clock.h"

App_Cpu0 g_AppCpu0;                               // Ƶ����Ϣ����

IFX_ALIGN(4) IfxCpu_syncEvent g_cpuSyncEvent = 0; // �¼�ͬ������

//-------------------------------------------------------------------------------------------------------------------
// �������          ����ϵͳƵ��
// ��������          void
// ʹ��ʾ��          set_clock();
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void set_clock (void)
{
    IfxScuCcu_setCpuFrequency(IfxCpu_ResourceCpu_0, (float32)AURIX_MCU_FREQUENCY);
    IfxScuCcu_setCpuFrequency(IfxCpu_ResourceCpu_1, (float32)AURIX_MCU_FREQUENCY);
}

//-------------------------------------------------------------------------------------------------------------------
// �������          ��ȡϵͳƵ��
// ��������          void
// ʹ��ʾ��          get_clock();
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void get_clock (void)
{
    // ��ȡʱ��Ƶ�ʣ����ڲ鿴��ǰϵͳ����Ƶ��
    g_AppCpu0.info.pllFreq = IfxScuCcu_getPllFrequency();
    g_AppCpu0.info.cpuFreq = IfxScuCcu_getCpuFrequency(IfxCpu_getCoreIndex());
    g_AppCpu0.info.sysFreq = IfxScuCcu_getSpbFrequency();
    g_AppCpu0.info.stmFreq = IfxStm_getFrequency(&MODULE_STM0);
}

//-------------------------------------------------------------------------------------------------------------------
// �������          �رտ��Ź�
// ��������          void
// ʹ��ʾ��          disable_Watchdog();
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void disable_Watchdog (void)
{
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());
}

//-------------------------------------------------------------------------------------------------------------------
// �������          ʱ�ӳ�ʼ��
// ��������          void
// ʹ��ʾ��          clock_init();
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void clock_init (void)
{
    interrupt_global_disable();     // �ر����ж�
    disable_Watchdog();             // �رտ��Ź�
    get_clock();                    // ��ȡϵͳƵ��
    interrupt_global_enable(0);     // ��ȫ���ж�
}


//-------------------------------------------------------------------------------------------------------------------
// �������          �ȴ����к��ĳ�ʼ�����
// ��������          void
// ʹ��ʾ��          cpu_wait_event_ready();
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void cpu_wait_event_ready (void)
{
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 0xFFFF);
}
