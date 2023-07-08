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
* �ļ�����          cpu1_main
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
#pragma section all "cpu1_dsram"
// ���������#pragma section all restore���֮���ȫ�ֱ���������CPU1��RAM��
#include "ImageConversion.h"
#include "ImageBasic.h"
#include "ImageTrack.h"
#include "bluetooth.h"
#include "ImageProcess.h"
#include "pid.h"
#include "KeyMenu.h"
#include "debug.h"
#include "icm20602.h"
#include "Control.h"
#include "ImageSpecial.h"
#include "isr.h"

// ���̵��뵽���֮��Ӧ��ѡ�й���Ȼ����refreshˢ��һ��֮���ٱ���
// ����Ĭ������Ϊ�ر��Ż��������Լ��һ�����ѡ��properties->C/C++ Build->Setting
// Ȼ�����Ҳ�Ĵ������ҵ�C/C++ Compiler->Optimization->Optimization level�������Ż��ȼ�
// һ��Ĭ���½����Ĺ��̶���Ĭ�Ͽ�2���Ż�����˴��Ҳ��������Ϊ2���Ż�
inline int Limit(int x, int low, int up)//��x���������޷�
{
    return x > up ? up : x < low ? low : x;
}
// ����TCϵ��Ĭ���ǲ�֧���ж�Ƕ�׵ģ�ϣ��֧���ж�Ƕ����Ҫ���ж���ʹ�� enableInterrupts(); �������ж�Ƕ��
// �򵥵�˵ʵ���Ͻ����жϺ�TCϵ�е�Ӳ���Զ������� disableInterrupts(); ���ܾ���Ӧ�κε��жϣ������Ҫ�����Լ��ֶ����� enableInterrupts(); �������жϵ���Ӧ��

// **************************** �������� ****************************
void core1_main(void)
{
    disable_Watchdog();                     // �رտ��Ź�
    interrupt_global_enable(0);             // ��ȫ���ж�
    // �˴���д�û����� ���������ʼ�������

    // �˴���д�û����� ���������ʼ�������
    cpu_wait_event_ready();                 // �ȴ����к��ĳ�ʼ�����
    sobel(mt9v03x_image,binary_image);
    otsu_thr=otsuThreshold(binary_image[0], MT9V03X_W, MT9V03X_H);//ʹ�ô�򷨵õ���ֵ����ֵ
    ReadFromFlash();//��flash�ж�ȡֵ
    base_speed=original_speed=process_speed[0];
    KeyCtrl();//��������
    pit_enable(CCU60_CH0);
    OutGarage();//����
    while (TRUE)
    {
        // �˴���д��Ҫѭ��ִ�еĴ���

        if(mt9v03x_finish_flag)
        {
#if 1
            memcpy(gray_image,mt9v03x_image,USE_IMAGE_H*USE_IMAGE_W);//37us
            //���籣��
            OutProtect();
            //ͼ����
            ImageProcess();
//            sobel(mt9v03x_image,binary_image);
            //��ʾ�Ҷ�ͼ
            ShowImage();
            //��ʾ����
            ShowLine();
            //����������Ϣ��������
            TrackBasicClear();
#else
//            memcpy(gray_image,mt9v03x_image,USE_IMAGE_H*USE_IMAGE_W);//37us
//            LCDShowPerImage();
//            MotorSetPWM(3000,3300);
            system_delay_ms(3300);
            WakeUpScreen();
//            while(1)
//            {
//                MotorSetPWM(0, 0);
//            }
//            seekfree_sendimg_03x(UART_2, mt9v03x_image[0], MT9V03X_W, MT9V03X_H);
#endif
            gpio_toggle_level(P20_8);//���İ��ϵ�led��ʾͼ��CPU1û�п���
            mt9v03x_finish_flag=0;
        }




         //�˴���д��Ҫѭ��ִ�еĴ���
    }
}
#pragma section all restore
