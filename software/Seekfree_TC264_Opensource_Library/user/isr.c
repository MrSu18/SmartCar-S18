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
* �ļ�����          isr
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

#include "isr_config.h"
#include "isr.h"
#include "icm20602.h"
#include "bluetooth.h"
#include "adc.h"
#include "motor.h"
#include "Fuzzypid.h"
#include "zf_device_tft180.h"
#include "ImageSpecial.h"
float icm_target_angle_z,icm_target_angle_x,icm_target_angle_y;   //������*����ֵ�Ŀ��Ƕ�

uint8 icm_angle_z_flag=0,icm_angle_x_flag=0,icm_angle_y_flag=0;     //������*����ִﵽĿ��Ƕ� ��־λ  ����Ϊ����������־λ //������
// **************************** PIT�жϺ��� ****************************
IFX_INTERRUPT(cc60_pit_ch0_isr, 0, CCU6_0_CH0_ISR_PRIORITY)//�ٶȻ�
{
    static uint32 time=0;
    time++;
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    pit_clear_flag(CCU60_CH0);

    MotorCtrl();
    if(time>2000)
    {
        pit_disable(CCU60_CH0);//�رյ���ж�
        pit_disable(CCU60_CH1);
        MotorSetPWM(0,0);
    }
}


IFX_INTERRUPT(cc60_pit_ch1_isr, 0, CCU6_0_CH1_ISR_PRIORITY)//ת��
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    pit_clear_flag(CCU60_CH1);

//    turnpid_image.err = image_bias;//ͼ��ƫ��
//    if (turnpid_image.out > 0)//��ת
//    {
//        target_left = base_speed - (int)2*turnpid_image.err;
//        target_right = base_speed;
//    }
//    else//��ת
//    {
//        target_left = base_speed;
//        target_right = base_speed + (int)2*turnpid_image.err;
//    }
    if(track_mode == kTrackImage)                                                   //��ǰΪ����ͷѭ��
    {
        if(last_track_mode == kTrackADC)                                            //��һ��ѭ��Ϊ���ѭ����λPID����
        {
            last_track_mode = track_mode;
            PIDClear();
        }

        FuzzyPID();                                                                 //����ͷ����PID
    }
    else if(track_mode == kTrackADC)                                                //��ǰΪ���ѭ��
    {
        if(last_track_mode == kTrackImage)                                          //��һ��ѭ��Ϊ����ͷѭ����λPID����
        {
            last_track_mode = track_mode;
            PIDClear();
        }

        FuzzyPID_ADC();
    }

    if(timeintegral_flag == 1)
        time++;
    c0h1_isr_flag=1;
}

IFX_INTERRUPT(cc61_pit_ch0_isr, 0, CCU6_1_CH0_ISR_PRIORITY)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��

    float angle_x=GetICM20602Angle_X(0);            //�ǶȻ���

      if(angle_x>icm_target_angle_x||angle_x<-icm_target_angle_x)  //�жϻ��ֽǶ��Ƿ����Ŀ��Ƕ�
     //if(my_angle_x>icm_target_angle_x||my_angle_x<-icm_target_angle_x)  //����Ͼ��bug ʹ�����
      {
         icm_angle_x_flag=1;                     //���ֵ���Ŀ��flag=1
         pit_disable(CCU61_CH0); //�ر��ж�

      }
    pit_clear_flag(CCU61_CH0);

}
int16 last_real_gyro = 0;
IFX_INTERRUPT(cc61_pit_ch1_isr, 0, CCU6_1_CH1_ISR_PRIORITY)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    pit_clear_flag(CCU61_CH1);
    float target_gyro = 5*turnpid_image.out;

    real_gyro = GetICM20602Gyro_X();
    real_gyro = 0.3 * real_gyro + 0.7*last_real_gyro;
    last_real_gyro = real_gyro;
    gyropid.err = target_gyro - real_gyro ;

    gyropid.out = gyropid.P * gyropid.err + gyropid.D * (gyropid.err - gyropid.last_err);
    gyropid.last_err = gyropid.err;

    target_left = base_speed - gyropid.out;
    target_right = base_speed + gyropid.out;
    gyro_flag = 1;
//    if(gyropid.out>0)//��ת
//    {
//        target_left = base_speed - gyropid.out;
//        target_right = base_speed;
//    }
//    else
//    {
//        target_left = base_speed;
//        target_right = base_speed + gyropid.out;
//    }
}
// **************************** PIT�жϺ��� ****************************


// **************************** �ⲿ�жϺ��� ****************************
IFX_INTERRUPT(exti_ch0_ch4_isr, 0, EXTI_CH0_CH4_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    if(exti_flag_get(ERU_CH0_REQ0_P15_4))           // ͨ��0�ж�
    {
        exti_flag_clear(ERU_CH0_REQ0_P15_4);
        wireless_module_uart_handler();                 // ����ģ��ͳһ�ص�����



    }

    if(exti_flag_get(ERU_CH4_REQ13_P15_5))          // ͨ��4�ж�
    {
        exti_flag_clear(ERU_CH4_REQ13_P15_5);




    }
}

IFX_INTERRUPT(exti_ch1_ch5_isr, 0, EXTI_CH1_CH5_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    if(exti_flag_get(ERU_CH1_REQ10_P14_3))          // ͨ��1�ж�
    {
        exti_flag_clear(ERU_CH1_REQ10_P14_3);



    }

    if(exti_flag_get(ERU_CH5_REQ1_P15_8))           // ͨ��5�ж�
    {
        exti_flag_clear(ERU_CH5_REQ1_P15_8);




    }
}

// ��������ͷpclk����Ĭ��ռ���� 2ͨ�������ڴ���DMA��������ﲻ�ٶ����жϺ���
// IFX_INTERRUPT(exti_ch2_ch6_isr, 0, EXTI_CH2_CH6_INT_PRIO)
// {
//  interrupt_global_enable(0);                     // �����ж�Ƕ��
//  if(exti_flag_get(ERU_CH2_REQ7_P00_4))           // ͨ��2�ж�
//  {
//      exti_flag_clear(ERU_CH2_REQ7_P00_4);
//  }
//  if(exti_flag_get(ERU_CH6_REQ9_P20_0))           // ͨ��6�ж�
//  {
//      exti_flag_clear(ERU_CH6_REQ9_P20_0);
//  }
// }

IFX_INTERRUPT(exti_ch3_ch7_isr, 0, EXTI_CH3_CH7_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    if(exti_flag_get(ERU_CH3_REQ6_P02_0))           // ͨ��3�ж�
    {
        exti_flag_clear(ERU_CH3_REQ6_P02_0);
        camera_vsync_handler();                     // ����ͷ�����ɼ�ͳһ�ص�����
    }
    if(exti_flag_get(ERU_CH7_REQ16_P15_1))          // ͨ��7�ж�
    {
        exti_flag_clear(ERU_CH7_REQ16_P15_1);




    }
}
// **************************** �ⲿ�жϺ��� ****************************


// **************************** DMA�жϺ��� ****************************
IFX_INTERRUPT(dma_ch5_isr, 0, DMA_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    camera_dma_handler();                           // ����ͷ�ɼ����ͳһ�ص�����
}
// **************************** DMA�жϺ��� ****************************


// **************************** �����жϺ��� ****************************
IFX_INTERRUPT(uart0_tx_isr, 0, UART0_TX_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    IfxAsclin_Asc_isrTransmit(&uart0_handle);


}
IFX_INTERRUPT(uart0_rx_isr, 0, UART0_RX_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    IfxAsclin_Asc_isrReceive(&uart0_handle);

#if DEBUG_UART_USE_INTERRUPT                        // ������� debug �����ж�
        debug_interrupr_handler();                  // ���� debug ���ڽ��մ����� ���ݻᱻ debug ���λ�������ȡ
#endif                                              // ����޸��� DEBUG_UART_INDEX ����δ�����Ҫ�ŵ���Ӧ�Ĵ����ж�ȥ

}
IFX_INTERRUPT(uart0_er_isr, 0, UART0_ER_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    IfxAsclin_Asc_isrError(&uart0_handle);



}

// ����1Ĭ�����ӵ�����ͷ���ô���
IFX_INTERRUPT(uart1_tx_isr, 0, UART1_TX_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    IfxAsclin_Asc_isrTransmit(&uart1_handle);




}
IFX_INTERRUPT(uart1_rx_isr, 0, UART1_RX_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    IfxAsclin_Asc_isrReceive(&uart1_handle);
    camera_uart_handler();                          // ����ͷ��������ͳһ�ص�����
}
IFX_INTERRUPT(uart1_er_isr, 0, UART1_ER_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    IfxAsclin_Asc_isrError(&uart1_handle);




}


// ����2Ĭ�����ӵ�����ת����ģ��
IFX_INTERRUPT(uart2_tx_isr, 0, UART2_TX_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    IfxAsclin_Asc_isrTransmit(&uart2_handle);





}
IFX_INTERRUPT(uart2_rx_isr, 0, UART2_RX_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    IfxAsclin_Asc_isrReceive(&uart2_handle);
    wireless_module_uart_handler();                 // ����ģ��ͳһ�ص�����

}
IFX_INTERRUPT(uart2_er_isr, 0, UART2_ER_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    IfxAsclin_Asc_isrError(&uart2_handle);

}



IFX_INTERRUPT(uart3_tx_isr, 0, UART3_TX_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    IfxAsclin_Asc_isrTransmit(&uart3_handle);




}
IFX_INTERRUPT(uart3_rx_isr, 0, UART3_RX_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    IfxAsclin_Asc_isrReceive(&uart3_handle);





}
IFX_INTERRUPT(uart3_er_isr, 0, UART3_ER_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    IfxAsclin_Asc_isrError(&uart3_handle);





}
// **************************** �����жϺ��� ****************************
