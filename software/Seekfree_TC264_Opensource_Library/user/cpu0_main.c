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
* �ļ�����          cpu0_main
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
#include "zf_common_headfile.h"
#pragma section all "cpu0_dsram"
// ���������#pragma section all restore���֮���ȫ�ֱ���������CPU0��RAM��
#include "zf_driver_gpio.h"
#include "motor.h"
#include "adc.h"
#include "filter.h"
#include "bluetooth.h"
#include "pid.h"
#include "KeyMenu.h"
#include "icm20602.h"
#include "Control.h"
#include "debug.h"
#include "FuzzyPID.h"
#include "ADRC.h"
#include "ImageProcess.h"

// ���̵��뵽���֮��Ӧ��ѡ�й���Ȼ����refreshˢ��һ��֮���ٱ���
// ����Ĭ������Ϊ�ر��Ż��������Լ��һ�����ѡ��properties->C/C++ Build->Setting
// Ȼ�����Ҳ�Ĵ������ҵ�C/C++ Compiler->Optimization->Optimization level�������Ż��ȼ�
// һ��Ĭ���½����Ĺ��̶���Ĭ�Ͽ�2���Ż�����˴��Ҳ��������Ϊ2���Ż�

// ����TCϵ��Ĭ���ǲ�֧���ж�Ƕ�׵ģ�ϣ��֧���ж�Ƕ����Ҫ���ж���ʹ�� interrupt_global_enable(0); �������ж�Ƕ��
// �򵥵�˵ʵ���Ͻ����жϺ�TCϵ�е�Ӳ���Զ������� interrupt_global_disable(); ���ܾ���Ӧ�κε��жϣ������Ҫ�����Լ��ֶ����� interrupt_global_enable(0); �������жϵ���Ӧ��

// �������ǿ�Դ����ֲ�ÿչ���
// �������ǿ�Դ����ֲ�ÿչ���
// �������ǿ�Դ����ֲ�ÿչ���

// **************************** �������� ****************************
int core0_main(void)
{
    clock_init();                   // ��ȡʱ��Ƶ��<��ر���>
    //debug_init();                   // ��ʼ��Ĭ�ϵ��Դ���
    // �˴���д�û����� ���������ʼ�������

/*************************ģ���ʼ��***************************/
    MotorInit();
    EncoderInit();
    ADCInit();
    KEYInit();
    mt9v03x_init();//��ʼ������ͷ
    tft180_init();//��ʼ��tft
    UARTInit();//��ʼ������ģ��
    icm20602_init();
    GyroOffsetInit();
    dl1a_init();
    //��ʼ��debug��led
    LedInit();
    //���������͵�ƽ
    gpio_init(BEER,GPO,GPIO_LOW,GPO_PUSH_PULL);
    //��ʼ���ж�
    pit_ms_init(CCU60_CH0,2);pit_disable(CCU60_CH0);//�ٶȻ�
    pit_ms_init(CCU61_CH1,4);pit_disable(CCU61_CH1);//���ٶȻ�
    pit_ms_init(CCU60_CH1,8);pit_disable(CCU60_CH1);//����
    //�������ж�2ms
    pit_ms_init(CCU61_CH0,2);pit_disable(CCU61_CH0);
/*************************������ʼ��***************************/
    ADRC_Init();
    KalmanInit(&kalman_adc,25,5);
    KalmanInit(&kalman_gyro,1,100);
    PIDInit(&speedpid_left,185.8,0.50,0);
    PIDInit(&speedpid_right,164.8,0.45,0);
    PIDInit(&turnpid_image,13,0,0);//700,40�н��ٶ�ֻ����50     10,3
    PIDInit(&turnpid_adc,10,0,12);
    PIDInit(&gyropid,-0.01125,-0.00024,-0);
    //ǰ������
    FFCInit(&speedffc_left,24255,716743,44.5);
    FFCInit(&speedffc_right,24409,628773,44.42);

    // �˴���д�û����� ���������ʼ�������
    cpu_wait_event_ready();         //�ȴ����к��ĳ�ʼ�����
//    ADCScan();
    while (TRUE)
    {
//        int16 a = GetICM20602Gyro_Y();
//        tft180_show_int(0, 0, a, 5);
//        dl1a_get_distance();
//        tft180_show_int(0, 0, dl1a_distance_mm, 5);
        ADCGetValue(adc_value);
//        ChaBiHe(TRACK);
//        if(gyro_flag == 1)
//        {
//            printf("%f,%d,%d,%f\n",image_bias,5000,real_gyro,gyropid.integer_err);
//            gyro_flag = 0;
//        }
        // �˴���д��Ҫѭ��ִ�еĴ���
//       if(c0h0_isr_flag==1)
//       {
//           printf("%d,%d,%f\r\n",base_speed,speed_type,image_bias);
//           c0h0_isr_flag=0;
//       }
       if(c0h1_isr_flag==1)
       {
           printf("%d,%f,%d,%d\r\n",s,image_bias,process_status_cnt,base_speed);
           c0h1_isr_flag=0;
       }
        // �˴���д��Ҫѭ��ִ�еĴ���
    }
}

#pragma section all restore
// **************************** �������� ****************************

