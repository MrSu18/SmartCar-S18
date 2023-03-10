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

Cross_Type cross_type = CROSS_NONE;
bool cross_found_l, cross_found_r;
int  corner_id_l = 0, corner_id_r = 0;
float distance = 0.0;                       //��ʱ�����жϣ��Ծ�����л��֣��ﵽһ������ʶ��Ϊ����ʮ��

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
    //static uint8 flag_in = 0, flag_end = 0;
    switch (cross_type)
    {
        case CROSS_NONE:
        {
            uint8 temp = CrossFindCorner();
            if (temp == 1)
            {
                aim_distance = ((float)(corner_id_l + corner_id_r)-5) * SAMPLE_DIST / 2;
                if (aim_distance < 0.05)
                {
                    cross_type = CROSS_BEGIN;
                    aim_distance = 0.16;
                }
            }
            break;
        }
        case CROSS_BEGIN:
        {
            gpio_toggle_level(P20_9);
            if (l_line_count < 2 && r_line_count < 2)
            {
                EdgeDetection_Cross();

                BlurPoints(left_line, l_line_count, f_left_line, LINE_BLUR_KERNEL);
                BlurPoints(right_line, r_line_count, f_right_line, LINE_BLUR_KERNEL);

                local_angle_points(f_left_line, l_line_count, l_angle, ANGLE_DIST / SAMPLE_DIST);
                local_angle_points(f_right_line, r_line_count, r_angle, ANGLE_DIST / SAMPLE_DIST);

                nms_angle(l_angle, l_line_count, l_angle_1, (ANGLE_DIST / SAMPLE_DIST) * 2 + 1);
                nms_angle(r_angle, r_line_count, r_angle_1, (ANGLE_DIST / SAMPLE_DIST) * 2 + 1);

                if(CrossFindCorner() == 0)
                {
                    while(1)
                    {
                        gpio_set_level(P21_4,0);
                        base_speed = 0;
                        image_bias=0;
                    }
                    cross_type = CROSS_IN;
                }
                else
                {
                    aim_distance = ((float)(corner_id_l + corner_id_r)+5) * SAMPLE_DIST / 2;
                }
            }
            break;
        }
        case CROSS_IN:
        {
//            if (l_line_count < 25 && r_line_count < 25)
//            {
//                uint8 equal_count = 0;
//                int i,j;
//                for (i = l_line_count,j = r_line_count; (i > l_line_count - 15) || (j > r_line_count - 15); i--, j--)
//                {
//                    if (f_left_line[i].X == f_right_line[j].X && f_left_line[i].Y == f_right_line[j].Y)
//                        equal_count++;
//                    else
//                        break;
//                }
//                if(equal_count == 15)
//                    cross_type = CROSS_IN_CUT;
//            }
            gpio_toggle_level(P21_5);
                break;
        }
        case CROSS_IN_CUT://�������ѭ�����ж��Ƿ�Ϊ��·���ڣ���������CROSS_OUT_CUT
        {
            break;
        }
        case CROSS_OUT_CUT://�л�����ͷѭ����ʶ���Ƿ�Ϊʮ�ֳ��ڣ�ͨ���ҽǵ㣬���ҵ�������CROSS_OUT
        {
            break;
        }
        case CROSS_OUT://����Ѳ�߷�ʽ��ͨ��ȫ�ײ��֣��ǵ㶪ʧ������CROSS_NONE
        {

            break;
        }
        default:break;
    }


    return 1;
}

/***********************************************
* @brief : ��ʮ�ֽǵ�
* @param : void
* @return: 0��û�нǵ�
*          1:�ҵ��ǵ�
* @date  : 2023.3.7
* @author: L
************************************************/
uint8 CrossFindCorner(void)
{
    cross_found_l = cross_found_r = false;

    for (int i = 0; i < l_line_count; i++)
    {
        if (l_angle_1[i] == 0) continue;
        if (cross_found_l == false && fabs(l_angle_1[i]) > 70 * 3.14 / 180 && fabs(l_angle_1[i]) < 120 * 3.14 / 180)
        {
            corner_id_l = i;
            cross_found_l = true;
        }
        else
           corner_id_l = 0;
    }
    for (int i = 0; i < r_line_count; i++)
    {
        if (r_angle_1[i] == 0) continue;
        if (cross_found_r == false && fabs(r_angle_1[i]) > 70 * 3.14 / 180 && fabs(r_angle_1[i]) < 120 * 3.14 / 180)
        {
            corner_id_r = i;
            cross_found_r = true;
        }
        else
            corner_id_r = 0;
    }
    if (cross_found_l && cross_found_r)
        return 1;
    else if ((cross_found_l && r_line_count < 4) || (cross_found_r && l_line_count < 4))
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
        else if (change_lr_flag == 1 && left_finish == 0)
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
                if (right_seed.Y == 0 || right_seed.X == right_border[right_seed.Y] || r_line_count >= EDGELINE_LENGTH - 1)//��������������ͼ���ϱ߽���ұ߽�˵��ɨ�������
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
                left_finish = 1;
            }
        }
        else break;
    } while (left_seed.Y != right_seed.Y || left_seed.X != right_seed.X);
}





