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

typedef enum CutType
{
    kCutIn = 0,
    kCutOut,
    kCutEnd,
}CutType;//��·״̬��״̬�ṹ��

CutType cut_type = kCutOut;

/***********************************************
* @brief : ��·״̬��
* @param : void
* @return: 0:���Ƕ�·
*          1:ʶ�𵽶�·
* @date  : 2023.5.5
* @author: L
************************************************/
uint8 CutIdentify(void)
{
    switch(cut_type)
    {
        case kCutIn:
        {
            int16 corner_id_l = 0,corner_id_r = 0;
            uint8 corner_find = CutFindCorner(&corner_id_l, &corner_id_r);//�ҽǵ�
            if(corner_find != 0)
            {
                base_speed = 50;
                //���ҽǵ㣬û����ǵ�
                if(corner_find == 2)
                {
                    per_r_line_count = (int)corner_id_r;//�޸ı��߳��ȵ��ǵ�λ��
                    track_rightline(f_right_line1, per_r_line_count, center_line_r, (int)round(ANGLE_DIST / SAMPLE_DIST), PIXEL_PER_METER * (TRACK_WIDTH / 2));
                    track_type = kTrackRight;
                    aim_distance = (float)(corner_id_r/2)*SAMPLE_DIST;
                }
                //����ǵ㣬û���ҽǵ�
                else if(corner_find == 1)
                {
                    per_l_line_count = (int)corner_id_l;//�޸ı��߳��ȵ��ǵ�λ��
                    track_leftline(f_left_line1, per_l_line_count, center_line_l, (int)round(ANGLE_DIST / SAMPLE_DIST), PIXEL_PER_METER * (TRACK_WIDTH / 2));
                    track_type = kTrackLeft;
                    aim_distance = (float)(corner_id_l/2)*SAMPLE_DIST;
                }
                //���߶��нǵ㣬�Ǳ߱��߳���Ѱ��������
                else if(corner_find == 3)
                {
                    if(per_l_line_count > per_r_line_count)
                    {
                        per_l_line_count = (int)corner_id_l;//�޸ı��߳��ȵ��ǵ�λ��
                        track_leftline(f_left_line1, per_l_line_count, center_line_l, (int)round(ANGLE_DIST / SAMPLE_DIST), PIXEL_PER_METER * (TRACK_WIDTH / 2));
                        track_type = kTrackLeft;
                        aim_distance = (float)(corner_id_l/2)*SAMPLE_DIST;
                    }
                    else if(per_r_line_count > per_l_line_count)
                    {
                        per_r_line_count = (int)corner_id_r;//�޸ı��߳��ȵ��ǵ�λ��
                        track_rightline(f_right_line1, per_r_line_count, center_line_r, (int)round(ANGLE_DIST / SAMPLE_DIST), PIXEL_PER_METER * (TRACK_WIDTH / 2));
                        track_type = kTrackRight;
                        aim_distance = (float)(corner_id_r/2)*SAMPLE_DIST;
                    }
                }
                //�л�״̬���ĳɵ��ѭ��
                if(corner_id_l < 50 && corner_id_r < 50)
                {
                    last_track_mode = track_mode;
                    track_mode = kTrackADC;
                    cut_type = kCutOut;
                    aim_distance = 0.45;
                }
            }
            break;
        }
        case kCutOut:
        {
            int over_count = 0;                                      //��ų�����ֵ����������
            for(int16 i = 0;i < MT9V03X_W;i++)                       //�������һ��
            {
                if(mt9v03x_image[106][i] <= OUT_THRESHOLD)
                        over_count++;
            }
            if(over_count>=MT9V03X_W-2)                             //���ȫ��������ֵ��ֹͣ
            {
                cut_type = kCutEnd;
            }
            break;
        }
        case kCutEnd:
        {
            //�������³��֣���·״̬�������л���ͼ��ѭ��
            if (l_line_count>50)
            {
                last_track_mode = track_mode;
                track_mode = kTrackImage;
                cut_type = kCutOut;//��λ״̬��
                base_speed = original_speed;
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
* @date  : 2023.4.21
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
