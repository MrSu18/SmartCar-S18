/*
 * Control.c
 *
 *  Created on: 2023��4��17��
 *      Author: 30516
 */

#include "Control.h"
#include "ImageTrack.h"
#include "math.h"
#include "zf_common_headfile.h"
#include "icm20602.h"
#include "motor.h"

//��ǰ���ٶȾ��߷�ʽ
enum SpeedType speed_type=kNormalSpeed;
//�趨���ٶ�
uint16 original_speed=65;

/***********************************************
* @brief : �ٶȾ���
* @param : uint16 original_speed:ԭʼ�ٶ�
*          float a: ���ٶ�
* @return: �ٶ�
* @date  : 2023.4.17
* @author: ������
************************************************/
uint16 SpeedDecision(uint16 original_speed,float a)
{
    int len=0;
    float* angle;
    switch (track_type)
    {
        case kTrackRight:
            angle=r_angle_1;
            len=per_r_line_count;
            break;
        case kTrackLeft:
            angle=l_angle_1;
            len=per_l_line_count;
            break;
        case kTrackSpecial:
            return original_speed;
        default:
            break;
    }
    //�ٶȼ���
    int s=0,i=3;
    uint16 vt=0;
    for(;i<len;i++)
    {
        float temp=fabs(angle[i]);
        if(temp>(5. / 180. * 3.14))
        {
            break;
        }
    }
    s=i-(int)(aim_distance/SAMPLE_DIST);//�õ�λ��
    vt= (uint16)sqrt(original_speed*original_speed+2*a*s);
//    if(vt>100) vt=100;//�޷�
    if(vt<original_speed) vt=original_speed;
    return vt;
}

/***********************************************
* @brief : д������
* @param : void
* @return: void
* @date  : 2023.4.19
* @author: ������
************************************************/
void OutGarage(void)
{
    encoder_dis_flag=1;
    image_bias=0.3;
    while(dis<200);//��ʱ�䳵����
    image_bias=5;    //�������
    StartIntegralAngle_X(70);
    while(!icm_angle_x_flag);   //��ת70���������Ѱ��
//    speed_type=kImageSpeed;//����֮�������ٶȾ���
}
