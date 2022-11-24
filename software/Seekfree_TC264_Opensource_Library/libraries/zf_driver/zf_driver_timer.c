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
* �ļ�����          zf_driver_timer
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

#include "IfxStm.h"
#include "IFXSTM_CFG.h"
#include "zf_driver_timer.h"

static uint32 systick_count[2];

//-------------------------------------------------------------------------------------------------------------------
//  �������      systick��ʱ������
//  ���ز���      void
//  ʹ��ʾ��      system_start(); // ������ʱ������¼�µ�ǰ��ʱ��
//-------------------------------------------------------------------------------------------------------------------
void system_start (void)
{
    systick_count[(IfxCpu_getCoreId())] = IfxStm_getLower(IfxStm_getAddress((IfxStm_Index)(IfxCpu_getCoreId())));
}

//-------------------------------------------------------------------------------------------------------------------
//  �������     ��õ�ǰSystem tick timer��ֵ
//  ���ز���     uint32           ���شӿ�ʼ�����ڵ�ʱ��(��λ10ns)
//  ʹ��ʾ��     uint32 tim = system_getval();
//  ��ע��Ϣ     �ں���0���ô˺�����ʹ��STM0ģ��  ����1��ʹ��STM1ģ��
//-------------------------------------------------------------------------------------------------------------------
uint32 system_getval (void)
{
    uint32 time;
    uint32 stm_clk;

    stm_clk = IfxStm_getFrequency(IfxStm_getAddress((IfxStm_Index)(IfxCpu_getCoreId())));

    time = IfxStm_getLower(IfxStm_getAddress((IfxStm_Index)(IfxCpu_getCoreId())));
    time = time - systick_count[(IfxCpu_getCoreId())];
    time = (uint32)((uint64)time * 100000000 / stm_clk);

    return time;
}




