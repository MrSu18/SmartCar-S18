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
* �ļ�����          zf_driver_encoder
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

#ifndef _zf_driver_encoder_h_
#define _zf_driver_encoder_h_

#include "zf_common_typedef.h"

// ��ö�ٶ��岻�����û��޸�
typedef enum // ö�ٱ���������
{
    TIM2_ENCOEDER_CH1_P00_7,      // T2��ʱ�� �������ſ�ѡ��Χ
    TIM2_ENCOEDER_CH1_P33_7,

    TIM3_ENCOEDER_CH1_P02_6,      // T3��ʱ�� �������ſ�ѡ��Χ

    TIM4_ENCOEDER_CH1_P02_8,      // T4��ʱ�� �������ſ�ѡ��Χ

    TIM5_ENCOEDER_CH1_P21_7,      // T5��ʱ�� �������ſ�ѡ��Χ
    TIM5_ENCOEDER_CH1_P10_3,

    TIM6_ENCOEDER_CH1_P20_3,      // T6��ʱ�� �������ſ�ѡ��Χ
    TIM6_ENCOEDER_CH1_P10_2,
}encoder_channel1_enum;

// ��ö�ٶ��岻�����û��޸�
typedef enum // ö�ٱ���������
{
    TIM2_ENCOEDER_CH2_P00_8,     // T2��ʱ�� �����������ſ�ѡ��Χ
    TIM2_ENCOEDER_CH2_P33_6,

    TIM3_ENCOEDER_CH2_P02_7,     // T3��ʱ�� �����������ſ�ѡ��Χ

    TIM4_ENCOEDER_CH2_P00_9,     // T4��ʱ�� �����������ſ�ѡ��Χ
    TIM4_ENCOEDER_CH2_P33_5,

    TIM5_ENCOEDER_CH2_P21_6,     // T5��ʱ�� �����������ſ�ѡ��Χ
    TIM5_ENCOEDER_CH2_P10_1,

    TIM6_ENCOEDER_CH2_P20_0,     // T6��ʱ�� �����������ſ�ѡ��Χ
}encoder_channel2_enum;

typedef enum  // ö�� ��ʱ�����
{
    TIM2_ENCOEDER,
    TIM3_ENCOEDER,
    TIM4_ENCOEDER,
    TIM5_ENCOEDER,
    TIM6_ENCOEDER,
}encoder_index_enum;

//====================================================������ ��������====================================================
int16 encoder_get_count     (encoder_index_enum encoder_n);
void  encoder_clear_count   (encoder_index_enum encoder_n);

void  encoder_quad_init     (encoder_index_enum encoder_n, encoder_channel1_enum count_pin, encoder_channel2_enum dir_pin);
void  encoder_dir_init      (encoder_index_enum encoder_n, encoder_channel1_enum ch1_pin, encoder_channel2_enum ch2_pin);
//====================================================������ ��������====================================================

#endif
