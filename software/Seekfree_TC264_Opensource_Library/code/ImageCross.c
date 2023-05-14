/*
 * ImageCross.c
 *
 *  Created on: 2023��3��10��
 *      Author: L
 */
#include "ImageSpecial.h"
#include "ImageTrack.h"
#include <math.h>
#include "ImageBasic.h"
#include "ImageProcess.h"
#include "motor.h"
#include "zf_device_tft180.h"

typedef enum CrossType
{
    kCrossBegin = 0,
    kCrossIn,
    kCrossOut,
}CrossType;//ʮ��״̬��״̬�ṹ��

CrossType cross_type = kCrossBegin;

/***********************************************
* @brief : ʮ��·��״̬��
* @param : void
* @return: 0:����ʮ��
*          1:ʶ��ʮ��
* @date  : 2023.3.21
* @author: L
************************************************/
uint8 CrossIdentify(void)
{
    switch (cross_type)
    {
     //ʶ���нǵ㣬�ұ������������߷ֿ��������Ԥ��㣬���ǵ���úܽ���ʱ���ʾ�Ѿ�Ҫ����ʮ�֣��л�״̬
    case kCrossBegin:
    {
        int16 corner_id_l = 0, corner_id_r = 0;         //�ǵ��ڱ��ߵĵڼ�����
        if (CrossFindCorner(&corner_id_l, &corner_id_r) != 0)
        {
            //�ǵ�̫Զ������״̬
            if((corner_id_l > aim_distance / SAMPLE_DIST) || (corner_id_r > aim_distance / SAMPLE_DIST)) break;
            //���û�нǵ㣬�ұ��нǵ㣬Ѱ���ߣ�����Ԥ���
            else if ((corner_id_l == 0) && (corner_id_r != 0))
            {
                track_type = kTrackRight;
                aim_distance = (float)(corner_id_r) * SAMPLE_DIST;
            }
            //�ұ�û�нǵ㣬����нǵ㣬Ѱ���ߣ�����Ԥ���
            else if ((corner_id_l != 0) && (corner_id_r == 0))
            {
                track_type = kTrackLeft;
                aim_distance = (float)(corner_id_l)* SAMPLE_DIST;
            }
            //���߶��ҽǵ㣬Ѱ���ߣ�����Ԥ���
            else
            {
                track_type = kTrackRight;
                aim_distance = (float)(corner_id_r) * SAMPLE_DIST;
            }
            //�ǵ�ܽ����л���һ��״̬
            if (corner_id_l < 6 && corner_id_r < 6)
            {
                encoder_dis_flag = 1;
                cross_type = kCrossIn;
                aim_distance = 0.45;
            }
        }
        break;
    }
    //Ĭ������ɨ�߲���ֲ��������ֵ��ͨ���ǵ��ж�Ѱ��һ�ߵı��ߣ����û���ҵ��ǵ㣬���ʾ�Ѿ�Ҫ����ʮ�֣��л�״̬
    case kCrossIn:
    {
        uint8 change_lr_flag = 0;               //�л�Ѱ�����ұ��߽ǵ�ı�־λ��Ĭ�������ҽǵ㣬û�ҵ����������
        uint8 corner_find = 0;                  //�Ƿ��ҵ��ǵ�ı�־λ

        if(l_line_count > 100 && r_line_count > 100)
        {
            cross_type = kCrossOut;
            aim_distance = 0.45;
        }

        EdgeDetection_Cross();
        per_r_line_count=PER_EDGELINE_LENGTH;   //����͸�Ӻ����ߵĳ���
        per_l_line_count=PER_EDGELINE_LENGTH;   //����͸�Ӻ����ߵĳ���
        //������͸��
        EdgeLinePerspective(left_line,l_line_count,per_left_line);
        EdgeLinePerspective(right_line,r_line_count,per_right_line);
        //�����˲�
        BlurPoints(per_left_line, l_line_count, f_left_line, LINE_BLUR_KERNEL);
        BlurPoints(per_right_line, r_line_count, f_right_line, LINE_BLUR_KERNEL);
        //���ߵȾ����
        ResamplePoints(f_left_line, l_line_count, f_left_line1, &per_l_line_count, SAMPLE_DIST*PIXEL_PER_METER);
        ResamplePoints(f_right_line, r_line_count, f_right_line1, &per_r_line_count, SAMPLE_DIST*PIXEL_PER_METER);
        //���������
        local_angle_points(f_left_line1,per_l_line_count,l_angle,ANGLE_DIST/SAMPLE_DIST);
        local_angle_points(f_right_line1,per_r_line_count,r_angle,ANGLE_DIST/SAMPLE_DIST);
        //���ʼ���ֵ����
        nms_angle(l_angle,per_l_line_count,l_angle_1,(ANGLE_DIST/SAMPLE_DIST)*2+1);
        nms_angle(r_angle,per_r_line_count,r_angle_1,(ANGLE_DIST/SAMPLE_DIST)*2+1);

        //�����߽ǵ�
        for (int i = 0; i < per_r_line_count; i++)
        {
            //�ҵ��ǵ���Ѱ���ߣ���Ѱ���ߣ��ı�Ԥ���
            if ((fabs(r_angle_1[i]) > 70 * 3.14 / 180) && (fabs(r_angle_1[i]) < 120 * 3.14 / 180))
            {
                if(i > 70) break;//�ǵ��Զ������
                else
                {
                    track_rightline(f_right_line1, per_r_line_count , center_line_r, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
                    track_type = kTrackRight;
                    aim_distance = (float)((i + 6) * SAMPLE_DIST);
                    change_lr_flag = 1;                             //����1�򲻴������ҽǵ�
                    corner_find = 1;                                //����0��Ĭ��Ѱ���ߣ�������Ѱ����
                    break;
                }
            }
        }
        //����û�ҵ��ǵ㣬��������
        if (change_lr_flag == 0)
        {
            for (int i = 0; i < per_l_line_count; i++)
            {
                //�ҵ��ǵ���Ѱ���ߣ��ı�Ԥ���
                if ((fabs(l_angle_1[i]) > 70 * 3.14 / 180) && (fabs(l_angle_1[i]) < 120 * 3.14 / 180))
                {
                    if(i > 70) break;//�ǵ��Զ������
                    else
                    {
                        track_rightline(f_right_line1, per_r_line_count , center_line_r, (int) round(ANGLE_DIST/SAMPLE_DIST), PIXEL_PER_METER*(TRACK_WIDTH/2));
                        track_type = kTrackLeft;
                        aim_distance = (float)((i + 6) * SAMPLE_DIST);
                        corner_find = 1;                                //����0��Ĭ��Ѱ���ߣ�������Ѱ����
                        break;
                    }
                }
            }
        }
        //û���ҵ��ǵ㣬Ĭ��Ѱ���ߣ��л���һ��״̬
//        if (corner_find == 0 && (left_line[0].Y > 100 || left_line[0].Y > 100))
//        {
//            track_rightline(f_right_line1, per_r_line_count, center_line_r, (int)round(ANGLE_DIST / SAMPLE_DIST), PIXEL_PER_METER * (TRACK_WIDTH / 2));
//            track_type = kTrackRight;
////            track_leftline(f_left_line1, per_l_line_count, center_line_l, (int)round(ANGLE_DIST / SAMPLE_DIST), PIXEL_PER_METER * (TRACK_WIDTH / 2));
////            track_type = kTrackLeft;
//            cross_type = kCrossOut;
//            aim_distance = 0.45;
//        }
        if(dis > 450)
        {
            encoder_dis_flag = 0;
            aim_distance = 0.45;
            cross_type = kCrossBegin;
            return 1;
        }

        break;
    }
    //�ж��Ƿ��Ѿ�����ʮ��
    case kCrossOut:
    {
        //�����ұ��߶�����10ʱ��ȷ���Ѿ�����ʮ�֣��˳�״̬����״̬����λ
        if (l_line_count > 10 && r_line_count > 10)
        {
            aim_distance = 0.45;
            cross_type = kCrossBegin;//��λ״̬��
            return 1;
        }
        break;
    }
    default:break;
    }
    return 0;
}

/***********************************************
* @brief : ��ʮ�ֽǵ�
* @param : corner_id_l:��ǵ��ڵڼ�����
*          corner_id_r:�ҽǵ��ڵڼ�����
* @return: 0��û�нǵ�
*          1:�ҵ������ǵ�
*          2:�ҵ�һ���ǵ㣬����һ�߶���
* @date  : 2023.3.21
* @author: L
************************************************/
uint8 CrossFindCorner(int16* corner_id_l, int16* corner_id_r)
{
    uint8 cross_find_l = FALSE, cross_find_r = FALSE;                       //�Ƿ��ҵ��ǵ��־λ

    //����ǵ�
    for (int16 i = 0; i < per_l_line_count; i++)
    {
        if (cross_find_l == FALSE &&((fabs(l_angle_1[i]) > 70 * 3.14 / 180) && (fabs(l_angle_1[i]) < 120 * 3.14 / 180)))
        {
            *corner_id_l = i;//����ǵ��ڱ��ߵڼ�����
            cross_find_l = TRUE;//�ҵ��ǵ��־λ
            break;
        }
    }
    //���ҽǵ�
    for (int16 i = 0; i < per_r_line_count; i++)
    {
        if (cross_find_r == FALSE && ((fabs(r_angle_1[i]) > 70 * 3.14 / 180) && (fabs(r_angle_1[i]) < 120 * 3.14 / 180)))
        {
            *corner_id_r = i;//����ǵ��ڱ��ߵڼ�����
            cross_find_r = TRUE;//�ҵ��ǵ��־λ
            break;
        }
    }

    //������߶��ҵ��ǵ㣬����1��һ���ҵ��ǵ�����һ�߶��ߣ�����2�����򷵻�0
    if (cross_find_l == TRUE && cross_find_r == TRUE)
    {
        if((f_left_line1[per_l_line_count - 1].X < f_left_line1[*corner_id_l].X) || (f_right_line1[per_r_line_count - 1].X > f_right_line1[*corner_id_r].X))
            return 1;
    }
    else if ((cross_find_l == TRUE) && (per_r_line_count < 5))
    {
        if(f_left_line1[per_l_line_count - 1].X < f_left_line1[*corner_id_l].X)
            return 2;
    }
    else if ((cross_find_r == TRUE) && (per_l_line_count < 5))
    {
        if(f_right_line1[per_r_line_count - 1].X > f_right_line1[*corner_id_r].X)
            return 3;
    }

    return 0;
}

/***********************************************
* @brief : ʮ��ɨ��
* @param : void
* @return: void
* @date  : 2023.4.14
* @author: L
************************************************/
void EdgeDetection_Cross(void)
{
    uint8 half=GRAY_BLOCK/2;
    myPoint left_seed, right_seed;              //��������
    left_seed = left_line[l_line_count - 1]; right_seed = right_line[r_line_count - 1];
    int now_point = 1,last_point = 1;
    uint8 is_loseline = 0;                      //�Ƿ��ߵı�־λ����������������������߾ͼ������

    if (left_seed.Y<USE_IMAGE_H_MIN+half || left_seed.Y>USE_IMAGE_H-half-1 || left_seed.X<half || left_seed.X>USE_IMAGE_W-half-1)
    {
        left_seed.Y=USE_IMAGE_H_MAX-half-1;left_seed.X=half;
    }
    left_seed.X++;
    if (right_seed.Y<USE_IMAGE_H_MIN+half || right_seed.Y>USE_IMAGE_H-half-1 || right_seed.X<half || right_seed.X>USE_IMAGE_W-half-1)
    {
        right_seed.Y=USE_IMAGE_H_MAX-half-1,right_seed.X=USE_IMAGE_W-half-1;
    }
    right_seed.X--;

    for (; left_seed.Y > USE_IMAGE_H_MAX - 45; left_seed.Y--)
    {
        last_point = now_point;
        now_point = PointSobelTest(left_seed);
        if (now_point == 1 && last_point == 0)
            break;
    }
    now_point=1;
    for (; right_seed.Y > USE_IMAGE_H_MAX - 45; right_seed.Y--)
    {
        last_point=now_point;
        now_point=PointSobelTest(right_seed);
        if (now_point == 1 && last_point == 0)
            break;
    }

    l_line_count = 0;r_line_count = 0;
    uint8 len_l=EDGELINE_LENGTH-(USE_IMAGE_H_MAX-left_seed.Y);
    uint8 len_r=EDGELINE_LENGTH-(USE_IMAGE_H_MAX-right_seed.Y);
    //���������
    while(l_line_count < len_l)
    {
        is_loseline = EightAreasSeedGrownGray(&left_seed,'l',&left_seed_num);
            if (is_loseline == 1)
            {
                left_line[l_line_count].X = left_seed.X;left_line[l_line_count].Y = left_seed.Y;
                l_line_count++;
            }
            else
                break;
    }
    //�ұ�������
    while(r_line_count < len_r)
    {
        is_loseline = EightAreasSeedGrownGray(&right_seed,'r',&right_seed_num);
            if (is_loseline == 1)
            {
                right_line[r_line_count].X = right_seed.X;right_line[r_line_count].Y = right_seed.Y;
                r_line_count++;
            }
            else
                break;
    }
}
