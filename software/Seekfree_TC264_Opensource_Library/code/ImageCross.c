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
}CrossType;

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
        if ((CrossFindCorner(&corner_id_l, &corner_id_r) != 0) && (f_left_line[l_line_count - 1].X < f_left_line[corner_id_l].X || f_right_line[r_line_count - 1].X > f_right_line[corner_id_r].X))
        {
            //���ֻ��һ���нǵ㣬������ƽ��ֵ������������ǵ㣬����ƽ��ֵ
            if ((corner_id_l == 0) && (corner_id_r != 0))
                aim_distance = (float)corner_id_r * SAMPLE_DIST;
            else if ((corner_id_l != 0) && (corner_id_r == 0))
                aim_distance = (float)corner_id_l * SAMPLE_DIST;
            else
                aim_distance = ((float)(corner_id_l + corner_id_r)) * SAMPLE_DIST / 2;

            //�ǵ�ܽ����л���һ��״̬
            if (corner_id_l < 8 && corner_id_r < 8)
                cross_type = kCrossIn;
        }
        else
            aim_distance = 0.45;
        break;
    }
    //Ĭ������ɨ�߲���ֲ��������ֵ��ͨ���ǵ��ж�Ѱ��һ�ߵı��ߣ����û���ҵ��ǵ㣬���ʾ�Ѿ�Ҫ����ʮ�֣��л�״̬
    case kCrossIn:
    {
        uint8 change_lr_flag = 0;               //�л�Ѱ�����ұ��߽ǵ�ı�־λ��Ĭ�������ҽǵ㣬û�ҵ����������
        uint8 corner_find = 0;                  //�Ƿ��ҵ��ǵ�ı�־λ

        EdgeDetection_Cross();

        BlurPoints(left_line, l_line_count, f_left_line, LINE_BLUR_KERNEL);
        BlurPoints(right_line, r_line_count, f_right_line, LINE_BLUR_KERNEL);

        local_angle_points(f_left_line, l_line_count, l_angle, ANGLE_DIST / SAMPLE_DIST);
        local_angle_points(f_right_line, r_line_count, r_angle, ANGLE_DIST / SAMPLE_DIST);

        nms_angle(l_angle, l_line_count, l_angle_1, (ANGLE_DIST / SAMPLE_DIST) * 2 + 1);
        nms_angle(r_angle, r_line_count, r_angle_1, (ANGLE_DIST / SAMPLE_DIST) * 2 + 1);

        //�����߽ǵ�
        for (int i = 0; i < r_line_count; i++)
        {
            //�ҵ��ǵ���Ѱ���ߣ���Ѱ���ߣ��ı�Ԥ���
            if ((fabs(r_angle_1[i]) > 70 * 3.14 / 180) && (fabs(r_angle_1[i]) < 120 * 3.14 / 180))
            {
                track_rightline(f_right_line, r_line_count, center_line_r, (int)round(ANGLE_DIST / SAMPLE_DIST), PIXEL_PER_METER * (TRACK_WIDTH / 2));
                track_type = kTrackRight;
                aim_distance = (float)i * SAMPLE_DIST;
                change_lr_flag = 1;                             //����1�򲻴������ҽǵ�
                corner_find = 1;                                //����0��Ĭ��Ѱ���ߣ�������Ѱ����
                break;
            }
        }
        //����û�ҵ��ǵ㣬��������
        if (change_lr_flag == 0)
        {
            for (int i = 0; i < l_line_count; i++)
            {
                //�ҵ��ǵ���Ѱ���ߣ��ı�Ԥ���
                if ((fabs(r_angle_1[i]) > 70 * 3.14 / 180) && (fabs(r_angle_1[i]) < 120 * 3.14 / 180))
                {
                    track_leftline(f_left_line, l_line_count, center_line_l, (int)round(ANGLE_DIST / SAMPLE_DIST), PIXEL_PER_METER * (TRACK_WIDTH / 2));
                    track_type = kTrackLeft;
                    aim_distance = (float)i * SAMPLE_DIST;
                    corner_find = 1;                                //����0��Ĭ��Ѱ���ߣ�������Ѱ������
                    break;
                }
            }
        }
        //û���ҵ��ǵ㣬Ĭ��Ѱ���ߣ��л���һ��״̬
        if (corner_find == 0)
        {
            track_rightline(f_right_line, r_line_count, center_line_r, (int)round(ANGLE_DIST / SAMPLE_DIST), PIXEL_PER_METER * (TRACK_WIDTH / 2));
            track_type = kTrackRight;
            cross_type = kCrossOut;
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
            cross_type = kCrossBegin;
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
    for (int16 i = 0; i < l_line_count; i++)
    {
        if (cross_find_l == FALSE &&((fabs(l_angle_1[i]) > 70 * 3.14 / 180) && (fabs(l_angle_1[i]) < 120 * 3.14 / 180)))
        {
            *corner_id_l = i;
            cross_find_l = TRUE;
            break;
        }
    }
    //���ҽǵ�
    for (int i = 0; i < r_line_count; i++)
    {
        if (cross_find_r == FALSE && ((fabs(r_angle_1[i]) > 70 * 3.14 / 180) && (fabs(r_angle_1[i]) < 120 * 3.14 / 180)))
        {
            cross_find_r = TRUE;
            break;
        }
    }

    //������߶��ҵ��ǵ㣬����1��һ���ҵ��ǵ�����һ�߶��ߣ�����2�����򷵻�0
    if (cross_find_l == TRUE && cross_find_r == TRUE)
        return 1;
    else if ((cross_find_l == TRUE && (r_line_count < 10)) || (cross_find_r == TRUE && (l_line_count < 10)))
        return 2;
    else
        return 0;
}

/***********************************************
* @brief : ʮ��ɨ��
* @param : void
* @return: void
* @date  : 2023.3.21
* @author: L
************************************************/
void EdgeDetection_Cross(void)
{
    myPoint left_seed, right_seed;              //��������
    left_seed = left_line[l_line_count - 1]; right_seed = right_line[r_line_count - 1];
    l_line_count = 0; r_line_count = 0;         //��ԭ��������
    uint8 change_lr_flag = 0, left_finish = 0, right_finish = 0;//change_lr_flag=0:������� change_lr_flag=1:�ұ�����
    uint8 is_loseline = 0;                      //�Ƿ��ߵı�־λ����������������������߾ͼ������
    do
    {
        if (change_lr_flag == 0 && left_finish == 0)
        {
            is_loseline = EightAreasSeedGrown(&left_seed, 'l', &left_seed_num);
            //���ߣ����������
            if (is_loseline == 2)
            {
                //�л�����Ѳ�ߵı�־����
                if (left_seed.Y == 0 || left_seed.X == right_border[left_seed.Y])//��������������ͼ���ϱ߽���ұ߽�˵��ɨ�������
                {
                    change_lr_flag = !change_lr_flag;
                    left_finish = 1;
                }
                else if (left_seed.X == left_border[left_seed.Y])//����������������߽�
                {
                    if (right_finish != 1)  change_lr_flag = !change_lr_flag;
                }
            }
            //�ҵ�����
            else if (is_loseline == 1)
            {
                left_line[l_line_count] = left_seed; l_line_count++;
                //�л�����Ѳ�ߵı�־����
                if (left_seed.Y == 0 || left_seed.X == right_border[left_seed.Y] || l_line_count >= EDGELINE_LENGTH - 1)//��������������ͼ���ϱ߽���ұ߽�˵��ɨ�������
                {
                    change_lr_flag = !change_lr_flag;
                    left_finish = 1;
                }
                else if (left_seed.X == left_border[left_seed.Y])//����������������߽�
                {
                    if (right_finish != 1)  change_lr_flag = !change_lr_flag;
                }
            }
            else
            {
                change_lr_flag = !change_lr_flag;
                left_finish = 1;
            }
        }
        else if (change_lr_flag == 1 && right_finish == 0)
        {
            is_loseline = EightAreasSeedGrown(&right_seed, 'r', &right_seed_num);
            //���ߣ����������
            if (is_loseline == 2)
            {
                //�л�����Ѳ�ߵı�־����
                if (right_seed.Y == 0 || right_seed.X == left_border[right_seed.Y])//��������������ͼ���ϱ߽���ұ߽�˵��ɨ�������
                {
                    change_lr_flag = !change_lr_flag;
                    right_finish = 1;
                }
                else if (right_seed.X == right_border[right_seed.Y])//����������������߽�
                {
                    if (left_finish != 1)  change_lr_flag = !change_lr_flag;
                }
            }
            //�ҵ�����
            else if (is_loseline == 1)
            {
                right_line[r_line_count] = right_seed; r_line_count++;
                //�л�����Ѳ�ߵı�־����
                if (right_seed.Y == 0 || right_seed.X == left_border[right_seed.Y] || r_line_count >= EDGELINE_LENGTH - 1)//��������������ͼ���ϱ߽���ұ߽�˵��ɨ�������
                {
                    change_lr_flag = !change_lr_flag;
                    right_finish = 1;
                }
                else if (right_seed.X == right_border[right_seed.Y])//����������������߽�
                {
                    if (left_finish != 1)  change_lr_flag = !change_lr_flag;
                }
            }
            else
            {
                change_lr_flag = !change_lr_flag;
                right_finish = 1;
            }
        }
        else break;
    } while (left_seed.Y != right_seed.Y || left_seed.X != right_seed.X);
}

