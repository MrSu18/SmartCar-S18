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
//            tft180_show_binary_image(0, 0, mt9v03x_image[0], USE_IMAGE_W, USE_IMAGE_H, 96, 60);
            EdgeDetection();
            //�Ա��߽����˲�
            myPoint_f f_left_line[EDGELINE_LENGTH],f_right_line[EDGELINE_LENGTH];
            BlurPoints(left_line, l_line_count, f_left_line, 5);
            BlurPoints(right_line, r_line_count, f_right_line, 5);
            //�Ⱦ����
            myPoint_f f_left_line1[EDGELINE_LENGTH],f_right_line1[EDGELINE_LENGTH];
            int l_count=200,r_count=200;
            ResamplePoints(f_left_line, l_line_count, f_left_line1, &l_count, 0.04*50);
            ResamplePoints(f_right_line, r_line_count, f_right_line1, &r_count, 0.04*50);
            //�ֲ�����
            float l_angle[l_count],r_angle[r_count];
            local_angle_points(f_left_line1,l_count,l_angle,0.1/0.04);
            local_angle_points(f_right_line1,r_count,r_angle,0.1/0.04);
            //����ֵ������
            float l_angle_1[l_count],r_angle_1[r_count];
            nms_angle(l_angle,l_count,l_angle_1,(0.1/0.04)*2+1);
            nms_angle(r_angle,r_count,r_angle_1,(0.1/0.04)*2+1);
            //�������ұ���
            track_leftline(f_left_line1, l_count, center_line_l, (int) round(0.1/0.04), 50*(0.4/2));
            track_rightline(f_right_line1, r_count, center_line_r, (int) round(0.1/0.04), 50*(0.4/2));
            cl_line_count=l_count;cr_line_count=r_count;
            // Ԥ�����ƫ��
            float Bias=GetAnchorPointBias(0.3,cr_line_count,center_line_r);
//            tft180_show_float(0, 0, Bias, 3, 3);
//            for(int i=0;i<cl_line_count;i++)
//            {
//                tft180_draw_point((uint16)center_line_l[i].X, (uint16)center_line_l[i].Y, RGB565_BLUE);
//            }
//            for(int i=0;i<cr_line_count;i++)
//            {
//                tft180_draw_point((uint16)center_line_r[i].X, (uint16)center_line_r[i].Y, RGB565_RED);
//            }
            TrackBasicClear();
            mt9v03x_finish_flag=0;
        }


        // �˴���д��Ҫѭ��ִ�еĴ���
    }
}
#pragma section all restore
