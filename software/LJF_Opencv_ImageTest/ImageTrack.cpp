//
// Created by 30516 on 2023/1/9.
//

#include "ImageTrack.h"
#include "math.h"
#include "stdint.h"
#include "stdio.h"

//ͼ��ѭ��ƫ��
float image_bias=0;
//Ԥ���
float aim_distance = 0.32;
//�任�����ұ���
myPoint_f per_left_line[EDGELINE_LENGTH]={0},per_right_line[EDGELINE_LENGTH]={0};
// �任�����ұ���+�˲�
myPoint_f f_left_line[EDGELINE_LENGTH]={0},f_right_line[EDGELINE_LENGTH]={0};
// �任�����ұ���+�Ⱦ����
myPoint_f f_left_line1[EDGELINE_LENGTH]={0},f_right_line1[EDGELINE_LENGTH]={0};
// ���ұ��߾ֲ��Ƕȱ仯��
float l_angle[EDGELINE_LENGTH]={0},r_angle[EDGELINE_LENGTH]={0};
// ���ұ��߾ֲ��Ƕȱ仯��+�Ǽ�������
float l_angle_1[EDGELINE_LENGTH]={0},r_angle_1[EDGELINE_LENGTH]={0};//���ұ��ߵķǼ���ֵ����֮��Ľǵ�����
// ��/������
myPoint_f center_line_l[EDGELINE_LENGTH]={0},center_line_r[EDGELINE_LENGTH]={0};//���ұ��߸��ٵõ�����������
// ��һ������
myPoint_f center_line[EDGELINE_LENGTH]={0};//��һ������
uint8 c_line_count=0;//��һ�����߳���
//��ǰ��Ѳ�߷���
enum TrackType track_type=kTrackRight;

inline int Limit(int x, int low, int up)//��x���������޷�
{
    return x > up ? up : x < low ? low : x;
}
inline float Min(float a,float b) {return a<b?a:b;}//��ab��Сֵ
inline float Max(float a,float b) {return a>b?a:b;}//��ab���ֵ

/***********************************************
* @brief : �Ա��߽�������ƽ���˲�
* @param : myPoint* in_line:ɨ�ߵõ�����ֵΪ���εı�������
 *         int num: ��������ĳ���
 *         myPoint_f* out_line: �˲�֮�󸡵����ݾ��ȵı�������
 *         uint8 kernel: �˲�һά���Ծ���˴�С������Ϊ������
* @return: ��
* @date  : 2023.1.10
* @author: �Ͻ���Դ
************************************************/
void BlurPoints(myPoint_f* in_line, int num, myPoint_f* out_line, uint8 kernel)
{
    int half = kernel / 2;
    float x=0,y=0;
    for (int i = 0; i < num; i++)
    {
        x = y = 0;
        for (int j = -half; j <= half; j++)
        {
            x += in_line[Limit(i + j, 0, num - 1)].X * (half + 1 - abs(j));
            y += in_line[Limit(i + j, 0, num - 1)].Y * (half + 1 - abs(j));
        }
        x /= (2 * half + 2) * (half + 1) / 2;
        y /= (2 * half + 2) * (half + 1) / 2;
        out_line[i].X=x;
        out_line[i].Y=y;
    }
}

/***********************************************
* @brief : �Ա��߽������µȾ����
* @param : myPoint_f* in_line:�����˲�֮��ı���
*          int num1:������ߵĳ���
*          myPoint_f* out_line: �Ⱦ������ı��ߣ���������֮���ʵ�ʾ�����ȣ�
*          int *num2: ������ߵĳ���
*          float dist: ������ʵ�ʾ��루��λcm��
* @return: ��
* @date  : 2023.1.12
* @author: �Ͻ���Դ
************************************************/
void ResamplePoints(myPoint_f* in_line, int num1, myPoint_f* out_line, uint8 *num2, float dist)
{
    //�����쳣���
    if (num1 < 0)
    {
        *num2 = 0;
        return;
    }
    out_line[0].X = in_line[0].X;
    out_line[0].Y = in_line[0].Y;
    int len = 1;//������ߵĳ��ȼ���ֵ
    //��ʼ�Ⱦ����
    for (int i = 0; i < num1 - 1 && len < *num2; i++)
    {
        float x0 = in_line[i].X;
        float y0 = in_line[i].Y;
        float x1 = in_line[i + 1].X;
        float y1 = in_line[i + 1].Y;

        do
        {
            float x = out_line[len - 1].X;
            float y = out_line[len - 1].Y;

            float dx0 = x0 - x;
            float dy0 = y0 - y;
            float dx1 = x1 - x;
            float dy1 = y1 - y;

            float dist0 = sqrt(dx0 * dx0 + dy0 * dy0);
            float dist1 = sqrt(dx1 * dx1 + dy1 * dy1);

            float r0 = (dist1 - dist) / (dist1 - dist0);
            float r1 = 1 - r0;

            if (r0 < 0 || r1 < 0) break;
            x0 = x0 * r0 + x1 * r1;
            y0 = y0 * r0 + y1 * r1;
            out_line[len].X = x0;
            out_line[len].Y = y0;
            len++;
        } while (len < *num2);

    }
    *num2 = len;
}

/***********************************************
* @brief : ���߾ֲ��Ƕȱ仯��
* @param : myPoint_f* in_line: �������
*          int num: ������ߵĳ���
*          float angle_out[]: �ֲ��Ƕȱ仯�����飨��λ�������ƣ�
*          int dist: �����������ʵ�����֮��ļ��������
* @return: ��
* @date  : 2023.1.15
* @author: �Ͻ���Դ
************************************************/
void local_angle_points(myPoint_f* in_line, int num, float angle_out[], int dist)
{
    for (int i = 0; i < num; i++)
    {
        if (i <= 0 || i >= num - 1)
        {
            angle_out[i] = 0;
            continue;
        }
        float dx1 = in_line[i].X - in_line[Limit(i - dist, 0, num - 1)].X;
        float dy1 = in_line[i].Y - in_line[Limit(i - dist, 0, num - 1)].Y;
        float dn1 = sqrtf(dx1 * dx1 + dy1 * dy1);
        float dx2 = in_line[Limit(i + dist, 0, num - 1)].X - in_line[i].X;
        float dy2 = in_line[Limit(i + dist, 0, num - 1)].Y - in_line[i].Y;
        float dn2 = sqrtf(dx2 * dx2 + dy2 * dy2);
        float c1 = dx1 / dn1;
        float s1 = dy1 / dn1;
        float c2 = dx2 / dn2;
        float s2 = dy2 / dn2;
        angle_out[i] = atan2f(c1 * s2 - c2 * s1, c2 * c1 + s2 * s1);
    }
}
/***********************************************
* @brief : �ǶȾֲ�����ֵ����
* @param : float angle_in[]���洢���ߵ�ÿ��ĽǶ�
*          int num�����鳤��
*          float angle_out[]������ֵ����֮�����������
*          int kernel���ֲ��󼫴�ֵ�ķ�Χ
* @return: ��
* @date  : 2023.1.15
* @author: �Ͻ���Դ
************************************************/
void nms_angle(float angle_in[], int num, float angle_out[], int kernel)
{
    int half = kernel / 2;
    for (int i = 0; i < num; i++)
    {
        angle_out[i] = angle_in[i];
        for (int j = -half; j <= half; j++)
        {
            //ǰ������о���ֵ���������ģ���ô����������λ0
            if (fabs(angle_in[Limit(i + j, 0, num - 1)]) > fabs(angle_out[i]))
            {
                angle_out[i] = 0;
                break;
            }
        }
    }
}

/***********************************************
* @brief : ����߰���ߵõ��ұ���
* @param : myPoint_f* in_line: ����ı���
*          int num: ������ߵĳ���
*          myPoint_f* out_line: ���������
*          int approx_num:����б��ʹ�õ�ǰ���ĸ���
*          float dist:���������
* @return: ��
* @date  : 2023.1.15
* @author: �Ͻ���Դ
************************************************/
void track_leftline(myPoint_f* in_line, int num, myPoint_f* out_line, int approx_num, float dist)
{
    for (int i = 0; i < num; i++)
    {
        float dx = in_line[Limit(i + approx_num, 0, num - 1)].X - in_line[Limit(i - approx_num, 0, num - 1)].X;
        float dy = in_line[Limit(i + approx_num, 0, num - 1)].Y - in_line[Limit(i - approx_num, 0, num - 1)].Y;
        float dn = sqrt(dx * dx + dy * dy);
        dx /= dn;
        dy /= dn;
        out_line[i].X = in_line[i].X - dy * dist;
        out_line[i].Y = in_line[i].Y + dx * dist;
    }
}

// �ұ��߸�������
void track_rightline(myPoint_f* in_line, int num, myPoint_f* out_line, int approx_num, float dist)
{
    for (int i = 0; i < num; i++)
    {
        float dx = in_line[Limit(i + approx_num, 0, num - 1)].X - in_line[Limit(i - approx_num, 0, num - 1)].X;
        float dy = in_line[Limit(i + approx_num, 0, num - 1)].Y - in_line[Limit(i - approx_num, 0, num - 1)].Y;
        float dn = sqrt(dx * dx + dy * dy);
        dx /= dn;
        dy /= dn;
        out_line[i].X = in_line[i].X + dy * dist;
        out_line[i].Y = in_line[i].Y - dx * dist;
    }
}

/***********************************************
* @brief : �õ�ѭ��Ԥê���ѭ��ƫ��
* @param : float aim_distance: Ԥ�����
*          uint8 track_line_count: �����ߵĳ���
*          myPoint_f *track_line: ���ٵı��ߣ������߻��������ߣ�
* @return: ��
* @date  : 2023.3.1
* @author: �Ͻ���Դ & ������
************************************************/
float GetAnchorPointBias(float aim_distance,uint8 track_line_count,myPoint_f *track_line)
{
    // ���ֶ�Ӧ��(��������ʼ��)
    float cx = USE_IMAGE_W/2;
    float cy = USE_IMAGE_H;
    float pure_angle=0;

    // �������(��ʼ�����߹�һ��)
    float min_dist = 1e10;
    int begin_id = -1;
    for (int i = 0; i < track_line_count; i++)
    {
        float dx = track_line[i].X - cx;
        float dy = track_line[i].Y - cy;
        float dist = sqrt(dx * dx + dy * dy);
        if (dist < min_dist)
        {
            min_dist = dist;
            begin_id = i;
        }
    }
    // �����е㣬ͬʱ����㲻����󼸸���
    if (begin_id >= 0 && track_line_count - begin_id >= 3)
    {
        // ��һ������
        track_line[begin_id].X = cx;
        track_line[begin_id].Y = cy;
        c_line_count = sizeof(center_line) / sizeof(center_line[0]);
        ResamplePoints(track_line + begin_id, track_line_count - begin_id, center_line, &c_line_count, SAMPLE_DIST*PIXEL_PER_METER);

        // ԶԤê��λ��
        int aim_idx = Limit(round(aim_distance / SAMPLE_DIST), 0, c_line_count - 1);

        // ����Զê��ƫ��ֵ
        float dx = center_line[aim_idx].X - cx;
        float dy = cy - center_line[aim_idx].Y + 0.2 * PIXEL_PER_METER;
        float dn = sqrt(dx * dx + dy * dy);
        //float error = -atan2f(dx, dy) * 180 / 3.14;

        // �������㷨(ֻ����Զ��)
        pure_angle = -atanf(PIXEL_PER_METER * 2 * 0.2 * dx / dn / dn) / 3.14 * 180 / 2.4;
    }
    else
    {
        // ���ߵ����(��������)���򲻿��ƶ��
        c_line_count = 0;
    }
    return pure_angle;
}

/***********************************************
* @brief : �������㲹�ߣ�ֱ�ߣ�
* @param : char choose
*          myPoint_f point1
*          myPoint_f point2
* @return: ��
* @date  : 2023.3.20
* @author: ������
************************************************/
void FillingLine(char choose, myPoint_f point1, myPoint_f point2)
{
    float K;//б��Ϊ�����ͣ�����K<1ʱ��K=0
    float B,Y,X;
    float start_row=Max(point1.Y,point2.Y),end_row=Min(point1.Y,point2.Y);
    /*�����������Ҫ��������һ�����ߵ�ʱ��*/
    if(point2.X == point1.X)
    {
        for (float Y = start_row; Y > end_row; Y--)
        {
            switch(choose)
            {
                case 'l':
                    f_left_line[l_line_count].X=X;f_left_line[l_line_count].Y=Y;l_line_count++;
                    break;
                case 'r':
                    f_right_line[r_line_count].X=X;f_right_line[r_line_count].Y=Y;r_line_count++;
                    break;
                default:break;
            }
            if (l_line_count>=EDGELINE_LENGTH && r_line_count>=EDGELINE_LENGTH)
                break;
        }
        return;
    }

    K= (-point2.Y + point1.Y) / (point2.X - point1.X);//k=(y2-y1)/(x2-x1)��ǿ������ת���������ʧ������ȻΪ0
    B= -point1.Y - K * point1.X;//b=y-kx

    for (float Y = start_row; Y > end_row; Y--)
    {
        X=((-Y-B)/K);          //ǿ������ת����ָ��������ʱ��ֻ��������
        X > USE_IMAGE_W-1 ? USE_IMAGE_W-1 : X < 0 ? 0 : X;
        switch(choose)
        {
            case 'l':
                f_left_line[l_line_count].X=X;f_left_line[l_line_count].Y=Y;l_line_count++;
                break;
            case 'r':
                f_right_line[r_line_count].X=X;f_right_line[r_line_count].Y=Y;r_line_count++;
                break;
            default:break;
        }
        if (l_line_count>=EDGELINE_LENGTH && r_line_count>=EDGELINE_LENGTH)
            break;
    }
}

/***********************************************
* @brief : ������͸��
* @param : myPoint* in_line: ԭͼɨ�ߵõ��ı���
*          uint8 num: �������鳤��
*          myPoint_f* out_line: ͸��֮��ı���
* @return: ��
* @date  : 2023.4.12
* @author: ������
************************************************/
void EdgeLinePerspective(myPoint* in_line,uint8 num,myPoint_f* out_line)
{
    double change_inverse_Mat[3][3]={{-0.3489,-2.1373,124.7762},{-0.0120,-3.2388,172.5025},{-0.0002,-0.0227, 1.0000}};
    for(uint8 count=0;count<num;count++)
    {
        float i=in_line[count].X;float j=in_line[count].Y;
        float solve_x = ((change_inverse_Mat[0][0]*i+change_inverse_Mat[0][1]*j+change_inverse_Mat[0][2])
                   /(change_inverse_Mat[2][0]*i+change_inverse_Mat[2][1]*j+change_inverse_Mat[2][2]));
        float solve_y = ((change_inverse_Mat[1][0]*i+change_inverse_Mat[1][1]*j+change_inverse_Mat[1][2])
                   /(change_inverse_Mat[2][0]*i+change_inverse_Mat[2][1]*j+change_inverse_Mat[2][2]));
        out_line[count].X = solve_x;
        out_line[count].Y = solve_y;
    }
}