/*
 * Control.c
 *
 *  Created on: 2023年4月17日
 *      Author: 30516
 */

#include "Control.h"
#include "ImageTrack.h"
#include "math.h"


/***********************************************
* @brief : 速度决策
* @param : uint16 original_speed:原始速度
*          float a: 加速度
* @return: 速度
* @date  : 2023.4.17
* @author: 刘骏帆
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
    //速度计算
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
    s=i;//得到位移
    vt= (uint16)sqrt(original_speed*original_speed+2*a*s);
//    if(vt>220) vt=220;//限幅
    return vt;
}


