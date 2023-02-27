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

// ���̵��뵽���֮��Ӧ��ѡ�й���Ȼ����refreshˢ��һ��֮���ٱ���
// ����Ĭ������Ϊ�ر��Ż��������Լ��һ�����ѡ��properties->C/C++ Build->Setting
// Ȼ�����Ҳ�Ĵ������ҵ�C/C++ Compiler->Optimization->Optimization level�������Ż��ȼ�
// һ��Ĭ���½����Ĺ��̶���Ĭ�Ͽ�2���Ż�����˴��Ҳ��������Ϊ2���Ż�

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
    while (TRUE)
    {
        // �˴���д��Ҫѭ��ִ�еĴ���

        if(mt9v03x_finish_flag)
        {
            ImageBinary();
            tft180_show_binary_image(0, 0, mt9v03x_image[0], USE_IMAGE_W, USE_IMAGE_H, 96, 60);
            EdgeDetection();
            //�Ա��߽����˲�
            myPoint_f f_left_line[EDGELINE_LENGTH],f_right_line[EDGELINE_LENGTH];
            BlurPoints(left_line, l_line_count, f_left_line, 7);
            BlurPoints(right_line, r_line_count, f_right_line, 7);
            //�Ⱦ����
            myPoint_f f_left_line1[EDGELINE_LENGTH],f_right_line1[EDGELINE_LENGTH];
            int l_count=200,r_count=200;
            ResamplePoints(f_left_line, l_line_count, f_left_line1, &l_count, 2);
            ResamplePoints(f_right_line, r_line_count, f_right_line1, &r_count, 2);
            //�ֲ�����
            float l_angle[l_count],r_angle[r_count];
            local_angle_points(f_left_line1,l_count,l_angle,5);
            local_angle_points(f_right_line1,r_count,r_angle,5);
            float l_angle_1[l_count],r_angle_1[r_count];
            nms_angle(l_angle,l_count,l_angle_1,5*2+1);
            nms_angle(r_angle,r_count,r_angle_1,5*2+1);
            //��������
            track_leftline(f_left_line1, l_count, center_line, 10, 22.5);
            for(int i=0;i<l_line_count;i++)
            {
                tft180_draw_point(f_left_line1[i].X, f_left_line1[i].Y, RGB565_GREEN);
            }
            for(int i=0;i<l_line_count;i++)
            {
                tft180_draw_point(f_right_line1[i].X, f_right_line1[i].Y, RGB565_YELLOW);
            }
            TrackBasicClear();
            mt9v03x_finish_flag=0;
        }


        // �˴���д��Ҫѭ��ִ�еĴ���
    }
}
#pragma section all restore
