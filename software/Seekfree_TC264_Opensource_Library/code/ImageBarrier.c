/*
 * ImageBarrier.c
 *
 *  Created on: 2023��4��15��
 *      Author: L
 */
#include "ImageSpecial.h"
#include "zf_device_dl1a.h"
#include "motor.h"
#include "ImageTrack.h"
#include "icm20602.h"
#include "Control.h"
#include "string.h"
#include "debug.h"
#include "zf_driver_gpio.h"
#include "adc.h"
#include "math.h"
#include "ImageProcess.h"

#define TOF_DISTANCE_THR 2000//TOF������ֵ
#define IMAGE_DISTANCE_THR  0.55//ͼ��յ����ֵ
#define ADC_IN_TRACK_THR    300//�жϳ��Ƿ��������ڵ�ADC��ֵ

uint8 barrier_status=0;//·��״̬
uint8 barrier_icm_flag=0;//·�����������Ǳ�־����

uint8 BarrierImageVerify(void);//·��ͼ�������֤

/***********************************************
* @brief : ʶ��·�ϵ�״̬��
* @param : void
* @return: 0:·��״̬��û����
*          1:·��״̬�Ѿ�����
* @date  : 2023.7.10
* @author: ������
************************************************/
uint8 BarrierIdentify(void)
{
    speed_type=kNormalSpeed;
    base_speed=original_speed;
    switch(barrier_status)
    {
        case 0://ʶ��·��
            if(BarrierImageVerify()==1)//ʶ�𵽹յ����·��
            {
                //tof�����ж�
                dl1a_get_distance();
                if(dl1a_distance_mm <= TOF_DISTANCE_THR && dl1a_finsh_flag==1)//tof��⵽·��
                {
                    barrier_status=1;
                    gpio_set_level(BEER, 1);//ʶ�𵽿�������
                    speed_type=kNormalSpeed;base_speed=process_property[process_status_cnt].min_speed;//����
                    track_type=kTrackSpecial;image_bias=6;//���
                    encoder_dis_flag=1;
                    StartIntegralAngle_X(50);while(!icm_angle_x_flag);//�ȴ�40��
                    image_bias=0;
                    while(dis<589.54);encoder_dis_flag=0;
                    track_type=kTrackSpecial;image_bias=-8;//�ҹ�
                    encoder_dis_flag=1;
                    StartIntegralAngle_X(90);while(!icm_angle_x_flag);//�ȴ�70��
                    image_bias=0;
                    while(dis<760.44);encoder_dis_flag=0;
//                    return 1;
                }
            }
            break;
        case 1://д���ճ�ȥ�ջ������жϴ�ʱ��������ǲ�����������
            //����ж��Ƿ��״̬
            if((L>ADC_IN_TRACK_THR && LM>ADC_IN_TRACK_THR) || M>ADC_IN_TRACK_THR || (RM>ADC_IN_TRACK_THR && R>ADC_IN_TRACK_THR))
            {
                gpio_set_level(BEER, 0);//״̬�����رշ�����
                speed_type=kImageSpeed;
                barrier_status=0;
                return 1;
            }
            //����б��ߣ������������������ģ�˵���������Ļص�������,Ѱ�����
            else if(l_line_count>20 && r_line_count<3 && (l_growth_direction[4]+l_growth_direction[3]-l_growth_direction[1]-l_growth_direction[0])>l_line_count-20)
            {
                track_type=kTrackLeft;
            }
            //����ж��Ƿ���������
            else if(L<ADC_IN_TRACK_THR && LM<ADC_IN_TRACK_THR && M<ADC_IN_TRACK_THR && RM<ADC_IN_TRACK_THR && R<ADC_IN_TRACK_THR)
            {
                //��֤�ұ����Ƿ�������Ҫ�ߵ����
                if(r_line_count>20 && (r_growth_direction[1]+r_growth_direction[0]-r_growth_direction[4]-r_growth_direction[3])>r_line_count-20)
                {
                    track_type=kTrackSpecial;//����������ƫ��
                    memcpy(left_line,right_line,2*r_line_count);//�ṹ���������ֽ�����*2
                    l_line_count=r_line_count;
                    EdgeLinePerspective(left_line,l_line_count,per_left_line);
                    BlurPoints(per_left_line, l_line_count, f_left_line, LINE_BLUR_KERNEL);
                    per_l_line_count=PER_EDGELINE_LENGTH;//��ֵ֮ǰҪ���������
                    ResamplePoints(f_left_line, l_line_count, f_left_line1, &per_l_line_count, SAMPLE_DIST*PIXEL_PER_METER);
                    track_leftline(f_left_line1, per_l_line_count, center_line_l, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(0.25));
                    image_bias = GetAnchorPointBias(aim_distance, per_l_line_count, center_line_l);
                }
            }
//            else//�������㣬����ɨ�ߵ���������,ƫ���0
//            {
//                track_type=kTrackSpecial;//����������ƫ��
//                image_bias=0;
//            }
            break;
        default:break;
    }
    return 0;
}

/***********************************************
* @brief : ʹ�ýǵ������֤�Ƿ���·��
* @param : void
* @return: 0:����·��
*          1:ʶ��·��
* @date  : 2023.7.5
* @author: ������
************************************************/
uint8 BarrierImageVerify(void)
{
    uint8 len=0;
    float* angle;
    //�������߱Ƚϳ���ȥ������������
    if(l_line_count>r_line_count)
    {
        len=l_line_count;
        angle=l_angle_1;
    }
    else
    {
        len=r_line_count;
        angle=r_angle_1;
    }
    //�������߲鿴�Ƿ��нǵ�
    for(uint8 i=0;i<len;i++)
    {
        float temp=fabs(angle[i]);
        if(temp>(70. / 180. * 3.14) && temp<(120. / 180. * 3.14) && i<IMAGE_DISTANCE_THR/SAMPLE_DIST)
        {
            return 1;
        }
    }
    return 0;
}
