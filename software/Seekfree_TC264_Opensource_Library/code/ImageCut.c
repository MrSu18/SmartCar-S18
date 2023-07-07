/*
 * ImageCut.c
 *
 *  Created on: 2023��3��20��
 *      Author: L
 */
#include "motor.h"
#include <math.h>
#include "ImageSpecial.h"
#include "ImageTrack.h"
#include "ImageBasic.h"
#include "ImageProcess.h"
#include "Control.h"
#include "motor.h"
#include "debug.h"
#include "zf_driver_gpio.h"

typedef enum CutType
{
    kCutBegin=0,
    kCutIn,
    kCutMid,
    kCutEndL,
    kCutEndR,
}CutType;//��·״̬��״̬�ṹ��

CutType cut_type = kCutBegin;
uint8 cut_flag=0;

/***********************************************
* @brief : ��·״̬��
* @param : void
* @return: 0:���Ƕ�·
*          1:ʶ�𵽶�·
* @date  : 2023.5.26
* @author: L
************************************************/
uint8 CutIdentify(void)
{
    if(now_flag == 0)
    {
        origin_aimdis = aim_distance;
        now_flag = 1;
    }
    switch(cut_type)
    {
        case kCutBegin://�ҽǵ㣬����һ���ǵ��ڱ���45��������ʱΪʶ�𵽶�·���л���һ��״̬
        {
            int16 corner_id_l = 0,corner_id_r = 0;
            if(CutFindCorner(&corner_id_l, &corner_id_r)!=0)
            {
                if((corner_id_l > 0.45 / SAMPLE_DIST) || (corner_id_r > 0.45 / SAMPLE_DIST))
                    break;
                else
                {
                    gpio_set_level(BEER,1);//����������
                    cut_type = kCutIn;
                }

            }
            break;
        }
        case kCutIn://���ǵ���40��������ʱ���л��ɵ��ѭ��������һ��״̬
        {
            int16 corner_id_l = 0,corner_id_r = 0;//�ǵ��ڱ��ߵڼ�����
            uint8 corner_find = CutFindCorner(&corner_id_l, &corner_id_r);//�ҽǵ�

            if(corner_find != 0)
            {
                CutChangeLine(corner_find, corner_id_r, corner_find);
                //�л�״̬���ĳɵ��ѭ��
                if(corner_id_l < 40 && corner_id_r < 40)
                {
                    cut_flag=1;//���ƫ���޷�
                    speed_type=kNormalSpeed;//�ر��ٶȾ���
                    base_speed = 60;//�������·
                    track_mode = kTrackADC;//�л����ѭ��
                    cut_type = kCutMid;
                    aim_distance = origin_aimdis;//�ָ�Ԥ���
                    encoder_dis_flag = 1;
                }
            }
            else
                aim_distance = origin_aimdis;
            break;
        }
        case kCutMid://���ܵ���·���棬��һ����
        {
            if(dis > 450)
            {
                encoder_dis_flag = 0;//�رձ���������
                cut_flag=0;//ȡ�����ƫ���޷�
                base_speed = 62;//���ڼ�һ����
                cut_type = kCutEndR;
            }
            break;
        }
        case kCutEndR://���ж��ұ����Ƿ����
        {
            //�ұ��߳��֣��л���һ��״̬
            if(r_line_count > 80 && right_line[r_line_count - 1].Y < 70)
                cut_type = kCutEndL;//�л���һ��״̬
            break;
        }
        case kCutEndL://���ж��ұ����Ƿ����
        {
            //�������³��֣���·״̬�������л���ͼ��ѭ��
            if (l_line_count > 80 && left_line[l_line_count - 1].Y < 70)
            {
                speed_type=kImageSpeed;//�����ٶȾ���
                track_mode = kTrackImage;//�л�ͼ��ѭ��
                cut_type = kCutBegin;//��λ״̬��
                gpio_set_level(BEER,0);//�رշ�����
                base_speed = original_speed;//�ָ��ٶ�
                now_flag = 0;
                return 1;
            }
            break;
        }
        default:break;
    }
    return 0;
}
/***********************************************
* @brief : ��·Ѱ�ҽǵ�
* @param : corner_id[2]:��ȡ�ǵ��ڱ��ߵĵڼ�����
* @return: corner_count:�ǵ������
* @date  : 2023.7.7
* @author: L
************************************************/
uint8 CutFindCorner(int16* corner_id_l,int16* corner_id_r)
{
    //��ǵ�
    for(int i = 0;i < per_l_line_count;i++)
    {
        if((fabs(l_angle_1[i]) > 70 * 3.14 / 180) && (fabs(l_angle_1[i]) < 120 * 3.14 / 180))
        {
            *corner_id_l = (int16)i;
            break;
        }
    }
    //�ҽǵ�
    for(int i = 0;i < per_r_line_count;i++)
    {
        if((fabs(r_angle_1[i]) > 70 * 3.14 / 180) && (fabs(r_angle_1[i]) < 120 * 3.14 / 180))
        {
            *corner_id_r = (int16)i;
            break;
        }
    }
    //ֻ�ҵ���ǵ�
    if(*corner_id_l != 0 && *corner_id_r == 0)
    {
        if(f_left_line1[per_l_line_count - 1].X > f_left_line1[*corner_id_l].X)
            return 1;
    }
    //ֻ�ҵ��ҽǵ�
    else if(*corner_id_l == 0 && *corner_id_r != 0)
    {
        if(f_right_line1[per_r_line_count - 1].X < f_right_line1[*corner_id_r].X)
            return 2;
    }
    //���߶��нǵ�
    else if(*corner_id_l != 0 && *corner_id_r != 0)
    {
        if((f_left_line1[per_l_line_count - 1].X > f_left_line1[*corner_id_l].X) || (f_right_line1[per_r_line_count - 1].X < f_right_line1[*corner_id_r].X))
            return 3;
    }
    //û�ҵ��ǵ�
    return 0;
}
/***********************************************
* @brief : ��·�л����ұ����ж�
* @param : corner_id_l:��ǵ��λ��
*          corner_id_r:�ҽǵ��λ��
*          corner_find:�ҵ������ĸ��ǵ�
* @return: void
* @date  : 2023.7.7
* @author: L
************************************************/
void CutChangeLine(int16 corner_id_l,int16 corner_id_r,uint8 corner_find)
{
    //���ҽǵ㣬û����ǵ�
    if(corner_find == 2)
    {
        per_r_line_count = (int)corner_id_r;//�޸ı��߳��ȵ��ǵ�λ��
        track_rightline(f_right_line1, per_r_line_count , center_line_r, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
        track_type = kTrackRight;
        aim_distance = (float)(corner_id_r/2)*SAMPLE_DIST;
    }
    //����ǵ㣬û���ҽǵ�
    else if(corner_find == 1)
    {
        per_l_line_count = (int)corner_id_l;//�޸ı��߳��ȵ��ǵ�λ��
        track_leftline(f_left_line1, per_l_line_count, center_line_l, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
        track_type = kTrackLeft;
        aim_distance = (float)(corner_id_l/2)*SAMPLE_DIST;
    }
    //���߶��нǵ㣬�ı߱��߳���Ѱ��������
    else if(corner_find == 3)
    {
        per_l_line_count = (int)corner_id_l;//�޸ı��߳��ȵ��ǵ�λ��
        track_leftline(f_left_line1, per_l_line_count, center_line_l, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
        track_type = kTrackLeft;
        aim_distance = (float)(corner_id_l/2)*SAMPLE_DIST;
    }

    if(aim_distance > origin_aimdis) aim_distance = origin_aimdis;//���Ԥ����ԭ����Զ��ʹ��ԭ����

    //��һ�ߵı��߲�������Ѱ����һ������
    if(r_growth_direction[7] > 25 && (r_growth_direction[0] + r_growth_direction[1]) > 20)
    {
        track_type = kTrackLeft;
    }
    else if(l_growth_direction[7] > 25 && (l_growth_direction[0] + l_growth_direction[1]) > 20)
    {
        track_type = kTrackRight;
    }
}
