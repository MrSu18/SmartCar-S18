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
#include "stdlib.h"

typedef enum CutType
{
    kCutBegin=0,
    kCutIn,
    kCutMid,
    kCutEndL,
    kCutEndR,
}CutType;//��·״̬��״̬�ṹ��

CutType cut_type = kCutBegin;

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
                //�л�״̬���ĳɵ��ѭ��
                if(corner_id_l < 40 && corner_id_r < 40)
                {
                    speed_type=kNormalSpeed;//�ر��ٶȾ���
                    base_speed = process_property[process_status_cnt].min_speed;//�������·
                    track_mode = kTrackADC;//�л����ѭ��
                    cut_type = kCutMid;
                    aim_distance = origin_aimdis;//�ָ�Ԥ���
//                    encoder_dis_flag = 1;
                }
                else//�������ͼ����
                {
                    CutChangeLine(corner_find, corner_id_r, corner_find);//�л�������
                }
            }
            else
                aim_distance = origin_aimdis;
            break;
        }
        case kCutMid://���ܵ���·���棬��һ����
        {
            if(r_line_count < 10 && l_line_count < 10)
            {
                base_speed = process_property[process_status_cnt].max_speed;//���ڼ�һ����
                cut_type = kCutEndR;
            }
            break;
        }
        case kCutEndR://���ж��ұ����Ƿ����
        {
            //�ұ��߳��֣��л���һ��״̬
            if(r_line_count > 80 && right_line[r_line_count - 1].Y < 70)
            {
                if(CutIgnoreNoise('r') == 1)
                    cut_type = kCutEndL;//�л���һ��״̬
                else
                    break;
            }
            break;
        }
        case kCutEndL://���ж��ұ����Ƿ����
        {
            //�������³��֣���·״̬�������л���ͼ��ѭ��
            if (l_line_count > 80 && left_line[l_line_count - 1].Y < 70)
            {
                if(CutIgnoreNoise('l') == 1)
                {
                    speed_type=kImageSpeed;//�����ٶȾ���
                    track_mode = kTrackImage;//�л�ͼ��ѭ��
                    cut_type = kCutBegin;//��λ״̬��
                    gpio_set_level(BEER,0);//�رշ�����
                    base_speed = original_speed;//�ָ��ٶ�
                    now_flag = 0;
                    return 1;
                }
                else
                    break;
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
        //���߲�������Ѱ���ߣ��ָ�Ԥ���
        if(r_growth_direction[7] > 25 && (r_growth_direction[0] + r_growth_direction[1]) > 20 && r_growth_direction[4] < 10)
        {
            track_type = kTrackLeft;
            aim_distance = origin_aimdis;
        }
        else//�����������ҽǵ���ڣ��޸�Ԥ��㣬Ѱ����
        {
            track_type = kTrackRight;
            //��Ԥ�������޸�
            if(corner_id_r<origin_aimdis/SAMPLE_DIST)
            {
                if(corner_id_r>3) aim_distance = (float)(corner_id_r-3)*SAMPLE_DIST;
            }

        }
    }
    //����ǵ㣬û���ҽǵ�
    else if(corner_find == 1)
    {
        //���߲�������Ѱ���ߣ��ָ�Ԥ���
        if(l_growth_direction[7] > 25 && (l_growth_direction[0] + l_growth_direction[1]) > 20 && l_growth_direction[4] < 10)
        {
            track_type = kTrackRight;
            aim_distance = origin_aimdis;
        }
        else
        {
            track_type = kTrackLeft;
            //��Ԥ�������޸�
            if(corner_id_l<origin_aimdis/SAMPLE_DIST)
            {
                if(corner_id_l>3) aim_distance = (float)(corner_id_l-3)*SAMPLE_DIST;
            }
        }
    }
    //���߶��нǵ㣬�ı߱��߳���Ѱ��������
    else if(corner_find == 3)
    {
        //����߲���������Ѱ�ұ���
        if(l_growth_direction[7] > 20 && (l_growth_direction[0] + l_growth_direction[1]) > 20 && l_growth_direction[4] < 10)
        {
            track_type = kTrackRight;
            //��Ԥ�������޸�
            if(corner_id_r<origin_aimdis/SAMPLE_DIST)
            {
                if(corner_id_r>3) aim_distance = (float)(corner_id_r-3)*SAMPLE_DIST;
            }
        }
        //�ұ��߲���������Ѱ�����
        else if(r_growth_direction[7] > 20 && (r_growth_direction[0] + r_growth_direction[1]) > 20 && r_growth_direction[4] < 10)//�޸ı��߳��ȵ��ǵ�λ��
        {
            track_type = kTrackLeft;
            //��Ԥ�������޸�
            if(corner_id_l<origin_aimdis/SAMPLE_DIST)
            {
                if(corner_id_l>3) aim_distance = (float)(corner_id_l-3)*SAMPLE_DIST;
            }
        }
        else
        {
            if(corner_id_l >= corner_id_r)//��ǵ���ҽǵ�Զ
            {
                track_type = kTrackLeft;
                //��Ԥ�������޸�
                if(corner_id_l<origin_aimdis/SAMPLE_DIST)
                {
                    if(corner_id_l>3) aim_distance = (float)(corner_id_l-3)*SAMPLE_DIST;
                }
            }
            else//�ҽǵ����ǵ�Զ
            {
                track_type = kTrackRight;
                //��Ԥ�������޸�
                if(corner_id_r<origin_aimdis/SAMPLE_DIST)
                {
                    if(corner_id_r>3) aim_distance = (float)(corner_id_r-3)*SAMPLE_DIST;
                }
            }
        }
    }

}
/***********************************************
* @brief : �ڶ�·������Ա���ΪһȦ������Ӱ��,������ǰ��״̬
* @param : lr_flag:ѡ�����ж�����߻����ұ���
* @return: 1:������ȷ
*          0:�����쳣
* @date  : 2023.7.8
* @author: L
************************************************/
uint8 CutIgnoreNoise(uint8 lr_flag)
{
    int grown_up = 0,grown_down = 0,grown_left = 0,grown_right = 0;//��������������
    switch(lr_flag)
    {
        case 'r':
        {
            grown_up = r_growth_direction[1] + r_growth_direction[2] + r_growth_direction[3];//�����ߵ�����
            grown_down = r_growth_direction[5] + r_growth_direction[6] + r_growth_direction[7];//�����ߵ�����
            grown_left = r_growth_direction[1] + r_growth_direction[0] + r_growth_direction[7];//�����ߵ�����
            grown_right = r_growth_direction[3] + r_growth_direction[4] + r_growth_direction[5];//�����ߵ�����
            //������Ϻ�����������������������30����������������������������������Ϊ���쳣����
            if(grown_up > 30 && grown_down > 30 && (abs(grown_right - grown_left) < 10))
                return 0;
            //������Һ�����������������������30���������Ϻ������������������������Ϊ���쳣����
            else if(grown_right > 30 && grown_left > 30 && (abs(grown_up - grown_left) < 10))
                return 0;
            return 1;
            break;
        }
        case 'l':
        {
            grown_up = l_growth_direction[1] + l_growth_direction[2] + l_growth_direction[3];//�����ߵ�����
            grown_down = l_growth_direction[5] + l_growth_direction[6] + l_growth_direction[7];//�����ߵ�����
            grown_left = l_growth_direction[1] + l_growth_direction[0] + l_growth_direction[7];//�����ߵ�����
            grown_right = l_growth_direction[3] + l_growth_direction[4] + l_growth_direction[5];//�����ߵ�����
            //������Ϻ�����������������������30����������������������������������Ϊ���쳣����
            if(grown_up > 30 && grown_down > 30 && (abs(grown_right - grown_left) < 10))
                return 0;
            //������Һ�����������������������30���������Ϻ������������������������Ϊ���쳣����
            else if(grown_right > 30 && grown_left > 30 && (abs(grown_up - grown_left) < 10))
                return 0;
            return 1;
            break;
        }
        default:break;
    }
    return 0;
}
