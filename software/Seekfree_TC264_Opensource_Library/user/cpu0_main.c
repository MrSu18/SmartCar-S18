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
#include "key.h"
//#include "icm20602.h"
#include "ZwPlus_lib.h"

extern uint8 binary_image[MT9V03X_H][MT9V03X_W];

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
//    uart_init(BLUETOOTH,3000000,BLUETOOTH_TX,BLUETOOTH_RX);//��ʼ��ͼ��
//    icm20602_init();
//    GyroOffsetInit();
//    dl1a_init();
    //��ʼ��debug��led
    gpio_init(P20_8, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    gpio_init(P20_9, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    gpio_init(P21_5, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    gpio_init(P21_4, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    //���������͵�ƽ
    gpio_init(P11_12,GPO,GPIO_HIGH,GPO_OPEN_DTAIN);
    //��ʼ������ж�
    pit_ms_init(CCU60_CH0,5);pit_disable(CCU60_CH0);
    pit_ms_init(CCU60_CH1,10);pit_disable(CCU60_CH1);
/*************************������ʼ��***************************/
    KalmanInit(&kalman_adc,25,5);
    PIDInit(&speedpid_left,187.52,1.16,0);              //187.52  1.16
    PIDInit(&speedpid_right,179.06,1.23,0);             //179.06  1.23
    PIDInit(&speedpid_left_1,168.46,0.86,0);            //244.24  1.99
    PIDInit(&speedpid_right_1,178.62,1.04,0);           //297.87  2.92
    PIDInit(&turnpid_image,0,0,0);                      //25   5
    PIDInit(&turnpid_adc,0,0,0);
    base_speed=150;

    // �˴���д�û����� ���������ʼ�������
    cpu_wait_event_ready();         // �ȴ����к��ĳ�ʼ�����

//    ADCScan();
    while (TRUE)
    {
        ADCGetValue(adc_value);
        // �˴���д��Ҫѭ��ִ�еĴ���
       if(c0h0_isr_flag==1)
       {
           printf("%d,%d,%d\r\n",base_speed,speed_left,speed_right);
           c0h0_isr_flag=0;
       }
//        if(mt9v03x_finish_flag)
//        {
//           gpio_toggle_level(P20_9);
//           Zw_SendImage(binary_image[0]);
//           system_delay_ms(10);
//        }
//       if(c0h1_isr_flag==1)
//       {
//           printf("%f,%d\r\n",turnpid_image.err,turnpid_image.out);
//           c0h1_isr_flag=0;
//       }
        // �˴���д��Ҫѭ��ִ�еĴ���
    }
}

#pragma section all restore
// **************************** �������� ****************************

