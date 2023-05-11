/*
 * Control.c
 *
 *  Created on: 2023年4月17日
 *      Author: 30516
 */

#include "Control.h"
#include "ImageTrack.h"
#include "math.h"
#include "zf_common_headfile.h"
#include "icm20602.h"
#include "motor.h"

//当前的速度决策方式
enum SpeedType speed_type=kNormalSpeed;
//设定的速度
uint16 original_speed=65;

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
    s=i-(int)(aim_distance/SAMPLE_DIST);//得到位移
    vt= (uint16)sqrt(original_speed*original_speed+2*a*s);
//    if(vt>100) vt=100;//限幅
    if(vt<original_speed) vt=original_speed;
    return vt;
}

/***********************************************
* @brief : 写死出库
* @param : void
* @return: void
* @date  : 2023.4.19
* @author: 刘骏帆
************************************************/
void OutGarage(void)
{
    encoder_dis_flag=1;
    image_bias=0.3;
    while(dis<200);//给时间车加速
    image_bias=5;    //向左打死
    StartIntegralAngle_X(70);
    while(!icm_angle_x_flag);   //左转70°进入正常寻迹
//    speed_type=kImageSpeed;//出库之后启动速度决策
}
