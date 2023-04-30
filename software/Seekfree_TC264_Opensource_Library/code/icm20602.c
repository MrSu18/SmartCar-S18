/*
 * icm20602.c
 *
 *  Created on: 2023年2月21日
 *      Author: L
 */
#include "icm20602.h"
#include "zf_device_icm20602.h"
#include "zf_driver_spi.h"
#include "filter.h"
#include "isr.h"
#include "zf_driver_pit.h"

#define delta_T 0.005f
#ifdef ICM_GET_Z
float my_angle_z=0;//Z轴角度
int16 my_gyro_z=0;//Z轴角速度
#endif
#ifdef ICM_GET_Y
float my_angle_y=0;
int16 my_gyro_y=0;
#endif
#ifdef ICM_GET_X
float my_angle_x=0;
int16 my_gyro_x=0;
#endif
QuaterInfo Q_info = {1,0,0,0};
EulerAngle eulerangle;
GyroOffset gyro_offset;
float I_ex=0, I_ey=0, I_ez=0;
float param_Kp = 50.0f;
float param_Ki = 0.2f;
float value[6]={0};


/***********************************************
* @brief : 初始化陀螺仪零漂
* @param : void
* @return: void
* @date  : 2023.2.21
* @author: L
************************************************/
void GyroOffsetInit(void)
{
    gyro_offset.x = 0;
    gyro_offset.y = 0;
    gyro_offset.z = 0;

    for(uint8 i = 0;i<100;i++)
    {
        icm20602_get_gyro();
        gyro_offset.x += icm20602_gyro_x;
        gyro_offset.y += icm20602_gyro_y;
        gyro_offset.z += icm20602_gyro_z;
        system_delay_ms(5);
    }

    gyro_offset.x /= 100;
    gyro_offset.y /= 100;
    gyro_offset.z /= 100;

    //printf("%d\n",gyro_offset.z);
}
/***********************************************
* @brief : 将采集到的数值转换成物理值，并对陀螺仪去零漂处理
* @param : value:存储转换后的值
* @return: void
* @date  : 2023.2.21
* @author: L
************************************************/
void ICMGetValue(float* value)
{
    float alpha = 0.35;
    static double lastacc[3] = {0,0,0};

    icm20602_get_acc();
    icm20602_get_gyro();

    //加速度转换成单位为 m/s^2
    value[0] = alpha*(float)icm20602_acc_x/4096 + (1-alpha)*lastacc[0];
    value[1] = alpha*(float)icm20602_acc_y/4096 + (1-alpha)*lastacc[1];
    value[2] = alpha*(float)icm20602_acc_z/4096 + (1-alpha)*lastacc[2];
    for(uint8 i = 0;i<3;i++)
    {
        lastacc[i] = value[i];
    }

    //角速度转换成单位为 rad/s
    value[3] = ((float)icm20602_gyro_x-gyro_offset.x)*PI/180/16.4f;
    value[4] = ((float)icm20602_gyro_y-gyro_offset.y)*PI/180/16.4f;
    value[5] = ((float)icm20602_gyro_z-gyro_offset.z)*PI/180/16.4f;
}
/***********************************************
* @brief : 互补滤波算法
* @param : gx,gy,gz:分别为化成物理值的加速度在x,y,z方向的值
*          ax,ay,az:分别为化成物理值的角速度在x,y,z方向的值
* @return: gyro_z:Z轴角速度
* @date  : 2023.2.21
* @author: L
************************************************/
void ICMUpdate(float gx,float gy,float gz,float ax,float ay,float az)
{
    float halfT = 0.5*delta_T;      //采样周期的一半
    float vx,vy,vz;                 //当前的机体坐标系上的重力单位向量
    float ex,ey,ez;                 //给陀螺仪的校正补偿值

    //四元数
    float q0 = Q_info.q0;
    float q1 = Q_info.q1;
    float q2 = Q_info.q2;
    float q3 = Q_info.q3;

    //后面需要用到的计算值
    float q0q0 = q0*q0;
    float q0q1 = q0*q1;
    float q0q2 = q0*q2;
    //float q0q3 = q0*q3;
    float q1q1 = q1*q1;
    //float q1q2 = q1*q2;
    float q1q3 = q1*q3;
    float q2q2 = q2*q2;
    float q2q3 = q2*q3;
    float q3q3 = q3*q3;

    //对加速度的值进行归一化，得到单位加速度
    float norm = sqrt(ax*ax+ay*ay+az*az);
    ax = ax/norm;
    ay = ay/norm;
    az = az/norm;

    //载体坐标系下重力在三个轴上的分量
    vx = 2*(q1q3-q0q2);
    vy = 2*(q2q3+q0q1);
    vz = q0q0-q1q1-q2q2+q3q3;

    //g^b和a^b叉乘，得到误差
    ex = vz*ay-vy*az;
    ey = vx*az-vz*ax;
    ez = vy*ax-vx*ay;

    //误差积分
    I_ex += delta_T*ex;
    I_ey += delta_T*ey;
    I_ez += delta_T*ez;

    //利用PI计算后的值补偿到陀螺仪
    gx = gx+param_Kp*ex+param_Ki*I_ex;
    gy = gy+param_Kp*ey+param_Ki*I_ey;
    gz = gz+param_Kp*ez+param_Ki*I_ez;

    //使用一阶龙格库塔求解四元数微分方程
    q0 = q0+(-q1*gx-q2*gy-q3*gz)*halfT;
    q1 = q1+(q0*gx-q2*gz-q3*gy)*halfT;
    q2 = q2+(q0*gy-q1*gz+q3*gx)*halfT;
    q3 = q3+(q0*gz+q1*gy-q2*gx)*halfT;

    //归一化
    norm = sqrt(q0*q0+q1*q1+q2*q2+q3*q3);

    Q_info.q0 = q0/norm;
    Q_info.q1 = q1/norm;
    Q_info.q2 = q2/norm;
    Q_info.q3 = q3/norm;
}
/***********************************************
* @brief : 把四元数换算成欧拉角
* @param : void
* @return: void
* @date  : 2023.2.22
* @author: L
************************************************/
void QuaterToEulerianAngles(void)
{
    ICMGetValue(value);
    ICMUpdate(value[3], value[4], value[5], value[0], value[1], value[2]);

    //四元数
    float q0 = Q_info.q0;
    float q1 = Q_info.q1;
    float q2 = Q_info.q2;
    float q3 = Q_info.q3;

    eulerangle.pitch = asin(-2*q1*q3+2*q0*q2)*180/PI;
    eulerangle.roll = atan2(2*q2*q3+2*q0*q1,-2*q1*q1-2*q2*q2+1)*180/PI;
    eulerangle.yaw = atan2(2*q1*q2+2*q0*q3,-2*q2*q2-2*q3*q3+1)*180/PI;
}
/***********************************************
* @brief : 获取ICM20602Z轴角速度
* @param : void
* @return: gyro_z:Z轴角速度
* @date  : 2023.2.21
* @author: L
************************************************/
int16 GetICM20602Gyro_Z(void)
{
    uint8 datz[2];
    int16 gyro_z;

    ICM20602_CS(0);
    spi_read_8bit_registers(SPI_0, ICM20602_GYRO_ZOUT_H | ICM20602_SPI_R, datz, 2);
    ICM20602_CS(1);

    gyro_z = (int16)(((uint16)datz[0]<<8)|datz[1]);

    return gyro_z;
}
int16 GetICM20602Gyro_X(void)
{
    uint8 datx[2];
    int16 gyro_x;

    ICM20602_CS(0);
    spi_read_8bit_registers(SPI_0, ICM20602_GYRO_XOUT_H | ICM20602_SPI_R, datx, 2);
    ICM20602_CS(1);

    gyro_x = (int16)(((uint16)datx[0]<<8)|datx[1]);

    return gyro_x;
}
int16 GetICM20602Gyro_Y(void)
{
    uint8 daty[2];
    int16 gyro_y;

    ICM20602_CS(0);
    spi_read_8bit_registers(SPI_0, ICM20602_GYRO_YOUT_H | ICM20602_SPI_R, daty, 2);
    ICM20602_CS(1);

    gyro_y = (int16)(((uint16)daty[0]<<8)|daty[1]);

    return gyro_y;
}

float GetICM20602Angle_Z(uint8 flag)
{


    if(flag==1) //清零之前的积分
    {
        my_angle_z=0;
        return 0;
    }

    my_gyro_z=GetICM20602Gyro_Z();                //获取Z轴角速度
    my_gyro_z=KalmanFilter(&kalman_gyro, my_gyro_z);  //滤波

    my_angle_z+=0.00012480f*my_gyro_z;                  //积分
  //  my_angle_z+=0.00012480f*value[5];                  //积分--误差滤波--近似于线性函数滤波
    return my_angle_z;
}
float GetICM20602Angle_X(uint8 flag)
{
    if(flag==1) //清零之前的积分
    {
        my_angle_x=0;
        return 0;
    }
    my_gyro_x=GetICM20602Gyro_X();                //获取X轴角速度
    my_gyro_x=KalmanFilter(&kalman_gyro, my_gyro_x);  //滤波
    my_angle_x+=0.00012480f*my_gyro_x;                  //积分  原0.00012480，现修正值为0.0005616
    return my_angle_x;
}
float GetICM20602Angle_Y(uint8 flag)
{
    if(flag==1) //清零之前的积分
    {
        my_angle_y=0;
        return 0;
    }
    my_gyro_y=GetICM20602Gyro_Y();                //获取Y轴角速度
    my_gyro_y=KalmanFilter(&kalman_gyro, my_gyro_y);  //滤波
    my_angle_y+=0.0005616f*my_gyro_y;                  //积分
    return my_angle_y;
}
void StartIntegralAngle_Z(float target_angle)
{
    icm_target_angle_z=target_angle;        //设置目标角度
    icm_angle_z_flag=0;                     //积分目标flag=0
    GetICM20602Angle_Z(1);                  //积分清零
    pit_enable(CCU61_CH0);  //开启中断
}
void StartIntegralAngle_X(float target_angle)
{
    icm_target_angle_x=target_angle;        //设置目标角度
    icm_angle_x_flag=0;                     //积分目标flag=0
    GetICM20602Angle_X(1);                  //积分清零
    pit_enable(CCU61_CH0);  //开启中断
}
void StartIntegralAngle_Y(float target_angle)
{
    icm_target_angle_y=target_angle;        //设置目标角度
    icm_angle_y_flag=0;                     //积分目标flag=0
    GetICM20602Angle_Y(1);                  //积分清零
    pit_enable(CCU61_CH0);  //开启中断
}

