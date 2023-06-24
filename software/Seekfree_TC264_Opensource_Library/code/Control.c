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

//��ͬ�ٶȶ�Ӧ�Ŀ��Ʋ���
ControlParam contro_param[10]={0};
//��ǰ���ٶȾ��߷�ʽ
enum SpeedType speed_type=kNormalSpeed;
//�趨���ٶ�
uint16 original_speed=65;
int s=0;//�ٶȾ��ߵ�λ��

/***********************************************
* @brief : �ٶȾ���
* @param : uint16 original_speed:ԭʼ�ٶ�
*          float a: ���ٶ�
* @return: �ٶ�
* @date  : 2023.6.23
* @author: ������
************************************************/
uint16 SpeedDecision(uint16 original_speed,float a)
{
    int len=0;
    if(image_bias>6 || image_bias<-6)   return original_speed;
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
    s=0;
    int i=3;
    uint16 vt=0;
    for(;i<len;i++)
    {
        float temp=fabs(angle[i]);
        if(temp>(10. / 180. * 3.14))
        {
            break;
        }
    }
    s=i-(int)(0.3/SAMPLE_DIST);//�õ�λ��
    //��ֱ�����⴦��
    if(s>=220)//������ʵ�����ݲɼ��õ�
    {
        vt=90;
    }
    else
    {
        vt= (uint16)sqrt(original_speed*original_speed+2*a*s);
        if(vt>80) vt=80;//�޷�
        if(vt<60) vt=60;
    }
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
    image_bias=0.5;
    while(dis<200);//��ʱ�䳵����
    encoder_dis_flag=0;
    image_bias=5;    //�������
    StartIntegralAngle_X(70);
    while(!icm_angle_x_flag);   //��ת70���������Ѱ��
    speed_type=kImageSpeed;//����֮�������ٶȾ���
}

/***********************************************
* @brief : ��ʼ����ͬ�ٶȵõ���ͬ�Ŀ��Ʋ���
* @param : void
* @return: void
* @date  : 2023.6.17
* @author: ������
************************************************/
void ControlParmInit(void)
{
    contro_param[0].speed=60;contro_param[0].turn_kp=13;contro_param[0].turn_kd=0;contro_param[0].turn_gkd=0.01;contro_param[0].aim=0.26;
    contro_param[1].speed=62;contro_param[1].turn_kp=13;contro_param[1].turn_kd=0;contro_param[1].turn_gkd=0.0125;contro_param[1].aim=0.26;
    contro_param[2].speed=65;contro_param[2].turn_kp=13;contro_param[2].turn_kd=1;contro_param[2].turn_gkd=0.0125;contro_param[2].aim=0.28;
}
