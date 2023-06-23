//
// Created by 30516 on 2023/4/17.
//
#include "Control.h"
#include "ImageTrack.h"
#include "math.h"

uint16 base_speed=65;

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
    s=i-(int)(0.5/SAMPLE_DIST);//�õ�λ��
    vt= (uint16)sqrt(original_speed*original_speed+2*a*s);
//    if(vt>70) vt=70;//�޷�
//    if(vt<original_speed) vt=original_speed;
    return vt;
}
