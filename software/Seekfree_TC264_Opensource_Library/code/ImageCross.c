/*
 * ImageCross.c
 *
 *  Created on: 2023��3��10��
 *      Author: L
 */
#include "ImageCross.h"
#include "ImageTrack.h"
#include <math.h>
#include "ImageBasic.h"
#include "ImageProcess.h"
#include "motor.h"
#include "zf_device_tft180.h"

Cross_Type cross_type = CROSS_IN;
Cut_Type cut_type = CUT_IN;

/***********************************************
* @brief : ��·״̬��
* @param : void
* @return: 0:����ʮ��·
*          1:ʶ�𵽶�·
* @date  : 2023.3.11
* @author: L
************************************************/
uint8 CutIdentify(void)
{
    switch (cut_type)
    {
    case CUT_IN:
    {
        int  corner_id_l = 0, corner_id_r = 0;
        int over_count = 0;

        for (int i = 0; i < MT9V03X_W; i++)
        {
            if (mt9v03x_image[MT9V03X_H - 1][i] <= 100)
                over_count++;
        }
        if (over_count >= MT9V03X_W)
        {
            gpio_set_level(P20_9,0);
            aim_distance = 0.16;
            cut_type = CUT_OUT;
        }
        else if (CrossFindCorner(&corner_id_l, &corner_id_r) == 1 && (f_left_line[corner_id_l + 4].X > f_left_line[corner_id_l].X || f_right_line[corner_id_r + 4].X < f_right_line[corner_id_r].X))
            aim_distance = ((float)(corner_id_l + corner_id_r)) * SAMPLE_DIST / 2;
        break;
    }
    case CUT_OUT:
    {
        if (l_line_count > 10 || r_line_count > 10)
        {
            gpio_set_level(P21_4,0);
            cut_type = CUT_IN;
            return 1;
        }
        break;
    }
    default:break;
    }
    return 0;
}

/***********************************************
* @brief : ʮ��·��״̬��
* @param : void
* @return: 0:����ʮ��
*          1:ʶ��ʮ��
* @date  : 2023.3.10
* @author: L
************************************************/
uint8 CrossIdentify(void)
{
    switch (cross_type)
    {
        case CROSS_IN:
        {
            int  corner_id_l = 0, corner_id_r = 0;
            if ( CrossFindCorner(&corner_id_l,&corner_id_r) == 1 &&                             \
                    (f_left_line[corner_id_l + 4].X < f_left_line[corner_id_l].X ||             \
                     f_right_line[corner_id_r + 4].X > f_right_line[corner_id_r].X))
            {
                aim_distance = ((float)(corner_id_l + corner_id_r)-5) * SAMPLE_DIST / 2;
                if (aim_distance < 0.05)
                {
                    cross_type = CROSS_OUT;
                    aim_distance = 0.16;
                }
            }
            break;
        }
        case CROSS_OUT:
        {
//            gpio_toggle_level(P20_9);
            if (l_line_count < 2 && r_line_count < 2)
            {
                int corner_id_l = 0, corner_id_r = 0;

                EdgeDetection_Cross();

                BlurPoints(left_line, l_line_count, f_left_line, LINE_BLUR_KERNEL);
                BlurPoints(right_line, r_line_count, f_right_line, LINE_BLUR_KERNEL);

                local_angle_points(f_left_line, l_line_count, l_angle, ANGLE_DIST / SAMPLE_DIST);
                local_angle_points(f_right_line, r_line_count, r_angle, ANGLE_DIST / SAMPLE_DIST);

                nms_angle(l_angle, l_line_count, l_angle_1, (ANGLE_DIST / SAMPLE_DIST) * 2 + 1);
                nms_angle(r_angle, r_line_count, r_angle_1, (ANGLE_DIST / SAMPLE_DIST) * 2 + 1);

                if(CrossFindCorner(&corner_id_l,&corner_id_r) == 0)
                {
//                    while(1)
//                    {
//                        gpio_set_level(P21_4,0);
//                        base_speed = 0;
//                        image_bias=0;
//                    }
                    cross_type = CROSS_IN;
                    return 1;
                }
                else
                    aim_distance = ((float)(corner_id_l + corner_id_r)+5) * SAMPLE_DIST / 2;
            }
            break;
        }
        default:break;
    }
    return 0;
}

/***********************************************
* @brief : ��ʮ�ֽǵ�
* @param : void
* @return: 0��û�нǵ�
*          1:�ҵ��ǵ�
* @date  : 2023.3.7
* @author: L
************************************************/
uint8 CrossFindCorner(int* corner_id_l,int* corner_id_r)
{
    bool cross_found_l = false, cross_found_r = false;

    for (int i = 0; i < l_line_count; i++)
    {
        if (l_angle_1[i] == 0) continue;
        if (cross_found_l == false && fabs(l_angle_1[i]) > 80 * 3.14 / 180 && fabs(l_angle_1[i]) < 110 * 3.14 / 180)
        {
//            tft180_show_float(100, 80,l_angle_1[i], 2, 3);
            *corner_id_l = i;
            cross_found_l = true;
        }
        else
           *corner_id_l = 0;
    }
    for (int i = 0; i < r_line_count; i++)
    {
        if (r_angle_1[i] == 0) continue;
        if (cross_found_r == false && fabs(r_angle_1[i]) > 80 * 3.14 / 180 && fabs(r_angle_1[i]) < 110 * 3.14 / 180)
        {
//            tft180_show_float(100, 100,r_angle_1[i], 2, 3);
            *corner_id_r = i;
            cross_found_r = true;
        }
        else
            *corner_id_r = 0;
    }
    if (cross_found_l && cross_found_r)
        return 1;
    else if ((cross_found_l && r_line_count < 5) || (cross_found_r && l_line_count < 5))
        return 1;
    else
        return 0;
}

/***********************************************
* @brief : ʮ��ɨ��
* @param : void
* @return: void
* @date  : 2023.3.8
* @author: L
************************************************/
void EdgeDetection_Cross(void)
{
    myPoint left_seed, right_seed;
    left_seed.X = left_line[l_line_count - 1].X; left_seed.Y = left_line[l_line_count - 1].Y; right_seed.X = right_line[r_line_count - 1].X; right_seed.Y = right_line[r_line_count - 1].Y;
    l_line_count = 0; r_line_count = 0;
    uint8 l_seed_num = 0, r_seed_num = 0;
    uint8 change_lr_flag = 0, left_finish = 0, right_finish = 0;//change_lr_flag=0:������� change_lr_flag=1:�ұ�����
    uint8 is_loseline = 0;
    do
    {
        if (change_lr_flag == 0 && left_finish == 0)
        {
            is_loseline = EightAreasSeedGrown(&left_seed, 'l', &l_seed_num);
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
            is_loseline = EightAreasSeedGrown(&right_seed, 'r', &r_seed_num);
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





