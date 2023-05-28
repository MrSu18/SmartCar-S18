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
    kCutBegin=0,
    kCutIn,
    kCutMid,
    kCutEnd,
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
        case kCutBegin:
        {
            int16 corner_id_l = 0,corner_id_r = 0;
            if(CutFindCorner(&corner_id_l, &corner_id_r)!=0)
            {
                if((corner_id_l > origin_aimdis / SAMPLE_DIST) || (corner_id_r > origin_aimdis / SAMPLE_DIST))
                    break;
                else
                    cut_type = kCutIn;
            }
            break;
        }
        case kCutIn:
        {
//            printf("1\n");
            int16 corner_id_l = 0,corner_id_r = 0;
            uint8 corner_find = CutFindCorner(&corner_id_l, &corner_id_r);//�ҽǵ�
            if(corner_find != 0)
            {
                //���ҽǵ㣬û����ǵ�
                if(corner_find == 2)
                {
                    per_r_line_count = (int)corner_id_r;//�޸ı��߳��ȵ��ǵ�λ��
                    track_type = kTrackRight;
                    aim_distance = (float)corner_id_r*SAMPLE_DIST;
                }
                //����ǵ㣬û���ҽǵ�
                else if(corner_find == 1)
                {
                    per_l_line_count = (int)corner_id_l;//�޸ı��߳��ȵ��ǵ�λ��
                    track_type = kTrackLeft;
                    aim_distance = (float)corner_id_l*SAMPLE_DIST;
                }
                //���߶��нǵ㣬�Ǳ߱��߳���Ѱ��������
                else if(corner_find == 3)
                {
                    if(per_l_line_count > per_r_line_count)
                    {
                        per_l_line_count = (int)corner_id_l;//�޸ı��߳��ȵ��ǵ�λ��
                        track_type = kTrackLeft;
                        aim_distance = (float)corner_id_l*SAMPLE_DIST;
                    }
                    else if(per_r_line_count > per_l_line_count)
                    {
                        per_r_line_count = (int)corner_id_r;//�޸ı��߳��ȵ��ǵ�λ��
                        track_type = kTrackRight;
                        aim_distance = (float)corner_id_r*SAMPLE_DIST;
                    }
                }
                //�л�״̬���ĳɵ��ѭ��
                if(corner_id_l < 40 && corner_id_r < 40)
                {
                    speed_type=kNormalSpeed;
                    base_speed = 55;
                    last_track_mode = track_mode;
                    track_mode = kTrackADC;
                    cut_type = kCutMid;
                    aim_distance = origin_aimdis;
                }
            }
            break;
        }
        case kCutMid:
        {
//            printf("2\n");
            int over_count = 0;
            for(int16 i = 0;i < MT9V03X_W;i++)
            {
                if(mt9v03x_image[106][i] <= OUT_THRESHOLD)
                        over_count++;
            }
            if(over_count>=MT9V03X_W-2)
                cut_type = kCutEnd;
            break;
        }
        case kCutEnd:
        {
//            printf("3\n");
            //�������³��֣���·״̬�������л���ͼ��ѭ��
            if (l_line_count>80 && r_line_count>80)
            {
//                printf("4\n");
                last_track_mode = track_mode;
                track_mode = kTrackImage;
                cut_type = kCutBegin;//��λ״̬��
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

