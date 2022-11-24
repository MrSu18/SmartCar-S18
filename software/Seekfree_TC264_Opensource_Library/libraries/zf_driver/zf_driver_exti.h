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
* ����              ����               ��ע
* 2022-09-15       pudding            first version
********************************************************************************************************************/

#ifndef _zf_driver_exti_h_
#define _zf_driver_exti_h_

#include "IfxScuEru.h"
#include "zf_common_typedef.h"

typedef enum  // ö��ERUͨ��
{
    // һ��ͨ��ֻ��ѡ������һ��������Ϊ �ⲿ�жϵ�����
    ERU_CH0_REQ0_P15_4   = 0*3 + 1,                                                 // ͨ��0��ѡ����
    ERU_CH1_REQ10_P14_3  = 1*3 + 1,                                                 // ͨ��1��ѡ����
    // �ر�ע��ͨ��2 �� ͨ��3��������ͷռ��
    // �ر�ע��ͨ��2 �� ͨ��3��������ͷռ��
    // �ر�ע��ͨ��2 �� ͨ��3��������ͷռ��
    ERU_CH2_REQ7_P00_4   = 2*3,  ERU_CH2_REQ14_P02_1, ERU_CH2_REQ2_P10_2,           // ͨ��2��ѡ����
    ERU_CH3_REQ6_P02_0   = 3*3,  ERU_CH3_REQ3_P10_3,  ERU_CH3_REQ15_P14_1,          // ͨ��3��ѡ����

    // ͨ��4��ͨ��0 �����жϺ��� ���ж���ͨ���жϱ�־λ��ʶ�����ĸ�ͨ���������ж�
    ERU_CH4_REQ13_P15_5  = 4*3,  ERU_CH4_REQ8_P33_7,                                // ͨ��4��ѡ����
    // ͨ��5��ͨ��1 �����жϺ���
    ERU_CH5_REQ1_P15_8   = 5*3,                                                     // ͨ��5��ѡ����
    // ͨ��6��ͨ��2 �����жϺ���
    ERU_CH6_REQ12_P11_10 = 6*3,  ERU_CH6_REQ9_P20_0,                                // ͨ��6��ѡ����
    // ͨ��7��ͨ��3 �����жϺ���
    ERU_CH7_REQ16_P15_1  = 7*3,  ERU_CH7_REQ11_P20_9,                               // ͨ��7��ѡ����
}exti_pin_enum;



typedef enum  // ö�ٴ�����ʽ
{
    EXTI_TRIGGER_RISING,                                                            // �����ش���ģʽ
    EXTI_TRIGGER_FALLING,                                                           // �½��ش���ģʽ
    EXTI_TRIGGER_BOTH,                                                              // ˫���ش���ģʽ
}exti_trigger_enum;



// �жϱ�־λ��ȡ
#define exti_flag_get(eru_pin)      IfxScuEru_getEventFlagStatus((IfxScuEru_InputChannel)(eru_pin/3))
// �жϱ�־λ���
#define exti_flag_clear(eru_pin)    IfxScuEru_clearEventFlag((IfxScuEru_InputChannel)(eru_pin/3))

//====================================================EXIT ��������====================================================
void exti_all_close         (void);                                                 // EXTI ʧ��
void exti_enable            (exti_pin_enum eru_pin);                                // EXTI �ж�ʹ��
void exti_disable           (exti_pin_enum eru_pin);                                // EXTI �ж�ʧ��
void exti_init              (exti_pin_enum eru_pin, exti_trigger_enum trigger);     // EXTI �жϳ�ʼ��
//====================================================EXIT ��������====================================================


#endif
