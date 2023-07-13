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
#include "ADRC.h"
#include "ImageProcess.h"

extern bool Outgarage_dir;//出库方向 0为左 1为右
//不同速度对应的控制参数
ControlParam contro_param[10]={0};
//当前的速度决策方式
enum SpeedType speed_type=kNormalSpeed;
//设定的速度
uint16 original_speed=65;
int s=0;//速度决策的位移
float speed_detection_a=9;//速度决策的加速度

/***********************************************
* @brief : 速度决策
* @param : uint16 original_speed:原始速度
*          float a: 加速度
* @return: 速度
* @date  : 2023.6.23
* @author: 刘骏帆
************************************************/
uint16 SpeedDecision(uint16 original_speed,float a)
{
    //偏差太大就恢复成设定速度
    if(image_bias>5 || image_bias<-5)   return original_speed;
    int len=0;
    float* angle;
    switch (track_type)
    {
        case kTrackRight:
            angle=r_angle;
            len=per_r_line_count;
            break;
        case kTrackLeft:
            angle=l_angle;
            len=per_l_line_count;
            break;
        case kTrackSpecial:
            return original_speed;
        default:
            break;
    }
    //速度计算
    s=0;
    int i=3,inflection_flag=0;
    uint16 vt=0;
    for(;i<len;i++)
    {
        float temp=fabs(angle[i]);
        if(temp>(10. / 180. * 3.14))
        {
            if((70. / 180. * 3.14)<temp && temp<(120. / 180. * 3.14) && i<50)//判断拐点
            {
                inflection_flag=1;
            }
            break;
        }
    }
    s=i-(int)(0.3/SAMPLE_DIST);//得到位移
    Fhan_ADRC(&adrc_speed_detection, (float)s);
    if(inflection_flag==1 && process_status[process_status_cnt]==3 && (image_bias<3 && image_bias>-3))//十字有拐点并且偏差很小的时候
    {
        vt=75;
    }
    else
    {
        vt= (uint16)sqrt(original_speed*((speed_left+speed_right)/2)+2*a*adrc_speed_detection.x1);//使用实时速度作为v0
//        vt= (uint16)sqrt(original_speed*original_speed+2*a*adrc_speed_detection.x1);//使用base_speeed作为v0
        //这里加权考虑偏差
        if(-1<image_bias && image_bias<1)
        {
            vt+=2;
        }
    }
    //速度限幅
    if(vt>90) vt=90;
    else if(vt<60) vt=60;
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
    switch(Outgarage_dir)
    {
        case 0://左转
            encoder_dis_flag=1;
            image_bias=0.5;
            while(dis<150);//给时间车加速
            encoder_dis_flag=0;
            image_bias=5;    //向左打死
            StartIntegralAngle_X(70);
            while(!icm_angle_x_flag);   //左转70°进入正常寻迹
            break;
        case 1://右转
            encoder_dis_flag=1;
            image_bias=-0.5;
            while(dis<150);//给时间车加速
            encoder_dis_flag=0;
            image_bias=-5;    //向左打死
            StartIntegralAngle_X(70);
            while(!icm_angle_x_flag);   //左转70°进入正常寻迹
            break;
        case 2://直走
            encoder_dis_flag=1;
            while(dis<500);//给时间车加速
            encoder_dis_flag=0;
            break;
        default:break;
    }
    speed_type=kImageSpeed;//出库之后启动速度决策
}

/***********************************************
* @brief : 初始化不同速度得到不同的控制参数
* @param : void
* @return: void
* @date  : 2023.6.17
* @author: 刘骏帆
************************************************/
void ControlParmInit(void)
{
    //右转的P是12
    contro_param[0].speed=60;contro_param[0].turn_kp=13;contro_param[0].turn_kd=0;contro_param[0].turn_gkd=0.01;contro_param[0].aim=0.32;
    contro_param[1].speed=62;contro_param[1].turn_kp=13;contro_param[1].turn_kd=0;contro_param[1].turn_gkd=0.01;contro_param[1].aim=0.32;
    contro_param[2].speed=64;contro_param[2].turn_kp=13;contro_param[2].turn_kd=0;contro_param[2].turn_gkd=0.01;contro_param[2].aim=0.32;
    contro_param[3].speed=66;contro_param[3].turn_kp=13;contro_param[3].turn_kd=5;contro_param[3].turn_gkd=0.01;contro_param[3].aim=0.32;
}
