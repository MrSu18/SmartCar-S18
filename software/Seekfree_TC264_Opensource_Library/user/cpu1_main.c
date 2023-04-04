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


    ImagePerspective_Init();//��͸�ӳ�ʼ��
    ImageBorderInit();//͸�Ӻ��ͼ��߽��ʼ��

    // �˴���д�û����� ���������ʼ�������
    cpu_wait_event_ready();                 // �ȴ����к��ĳ�ʼ�����

    KEYCtrl();//��������

    while (TRUE)
    {
        // �˴���д��Ҫѭ��ִ�еĴ���

        if(mt9v03x_finish_flag)
        {
#if 1
            //���籣��
            OutProtect();

            if(gray_image_flag == 1)
                tft180_show_gray_image(0, 0, mt9v03x_image[0], MT9V03X_W, MT9V03X_H, 160, 120, 0);

            ImageBinary();
            //��ʾͼ��
            if(binary_image_flag == 1)
            {
                tft180_show_binary_image(0, 0, mt9v03x_image[0], USE_IMAGE_W, USE_IMAGE_H, 94, 60);
                tft180_show_float(98, 0, image_bias, 2, 3);
            }
            gpio_toggle_level(P20_8);

            //ͼ����
            ImageProcess();

            //��ʾ���ߺ�����
            if(edgeline_flag == 1)
            {
                for(int i=0;i<l_line_count;i++)
                {
                    tft180_draw_point((uint16)f_left_line[i].X, (uint16)f_left_line[i].Y, RGB565_BLUE);
                }
                for(int i=0;i<r_line_count;i++)
                {
                    tft180_draw_point((uint16)f_right_line[i].X, (uint16)f_right_line[i].Y, RGB565_RED);
                }
                tft180_show_int(98, 30, l_line_count, 3);
                tft180_show_int(98, 60, r_line_count, 3);
            }
            if(c_line_flag == 1)
            {
                for(int i=0;i<Limit(round(aim_distance / SAMPLE_DIST), 0, c_line_count);i++)
                {
                    tft180_draw_point((uint16)center_line[i].X, (uint16)center_line[i].Y, RGB565_RED);
                }
            }

            //����������Ϣ��������
            TrackBasicClear();
#else
//            MotorSetPWM(1500,3500);
//            system_delay_ms(3000);
//            while(1)
//            {
//                MotorSetPWM(0, 0);
//            }
            seekfree_sendimg_03x(UART_2, mt9v03x_image[0], MT9V03X_W, MT9V03X_H);
#endif
            mt9v03x_finish_flag=0;
        }




         //�˴���д��Ҫѭ��ִ�еĴ���
    }
}
#pragma section all restore
