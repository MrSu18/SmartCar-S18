/*
 * icm20602.c
 *
 *  Created on: 2023��2��21��
 *      Author: L
 */
#include "icm20602.h"
#include "zf_device_icm20602.h"
#include "zf_driver_spi.h"
#include "filter.h"

#define delta_T 0.005f

QuaterInfo Q_info = {1,0,0,0};
EulerAngle eulerangle;
GyroOffset gyro_offset;
float I_ex, I_ey, I_ez;
float param_Kp = 50.0f;
float param_Ki = 0.2f;
float value[6];

/***********************************************
* @brief : ��ʼ����������Ư
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
* @brief : ���ɼ�������ֵת��������ֵ������������ȥ��Ư����
* @param : value:�洢ת�����ֵ
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

    //���ٶ�ת���ɵ�λΪ m/s^2
    value[0] = alpha*(float)icm20602_acc_x/4096 + (1-alpha)*lastacc[0];
    value[1] = alpha*(float)icm20602_acc_y/4096 + (1-alpha)*lastacc[1];
    value[2] = alpha*(float)icm20602_acc_z/4096 + (1-alpha)*lastacc[2];
    for(uint8 i = 0;i<3;i++)
    {
        lastacc[i] = value[i];
    }

    //���ٶ�ת���ɵ�λΪ rad/s
    value[3] = ((float)icm20602_gyro_x-gyro_offset.x)*PI/180/16.4f;
    value[4] = ((float)icm20602_gyro_y-gyro_offset.y)*PI/180/16.4f;
    value[5] = ((float)icm20602_gyro_z-gyro_offset.z)*PI/180/16.4f;
}
/***********************************************
* @brief : �����˲��㷨
* @param : gx,gy,gz:�ֱ�Ϊ��������ֵ�ļ��ٶ���x,y,z�����ֵ
*          ax,ay,az:�ֱ�Ϊ��������ֵ�Ľ��ٶ���x,y,z�����ֵ
* @return: gyro_z:Z����ٶ�
* @date  : 2023.2.21
* @author: L
************************************************/
void ICMUpdate(float gx,float gy,float gz,float ax,float ay,float az)
{
    float halfT = 0.5*delta_T;      //�������ڵ�һ��
    float vx,vy,vz;                 //��ǰ�Ļ�������ϵ�ϵ�������λ����
    float ex,ey,ez;                 //�������ǵ�У������ֵ

    //��Ԫ��
    float q0 = Q_info.q0;
    float q1 = Q_info.q1;
    float q2 = Q_info.q2;
    float q3 = Q_info.q3;

    //������Ҫ�õ��ļ���ֵ
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

    //�Լ��ٶȵ�ֵ���й�һ�����õ���λ���ٶ�
    float norm = sqrt(ax*ax+ay*ay+az*az);
    ax = ax/norm;
    ay = ay/norm;
    az = az/norm;

    //��������ϵ���������������ϵķ���
    vx = 2*(q1q3-q0q2);
    vy = 2*(q2q3+q0q1);
    vz = q0q0-q1q1-q2q2+q3q3;

    //g^b��a^b��ˣ��õ����
    ex = vz*ay-vy*az;
    ey = vx*az-vz*ax;
    ez = vy*ax-vx*ay;

    //������
    I_ex += delta_T*ex;
    I_ey += delta_T*ey;
    I_ez += delta_T*ez;

    //����PI������ֵ������������
    gx = gx+param_Kp*ex+param_Ki*I_ex;
    gy = gy+param_Kp*ey+param_Ki*I_ey;
    gz = gz+param_Kp*ez+param_Ki*I_ez;

    //ʹ��һ��������������Ԫ��΢�ַ���
    q0 = q0+(-q1*gx-q2*gy-q3*gz)*halfT;
    q1 = q1+(q0*gx-q2*gz-q3*gy)*halfT;
    q2 = q2+(q0*gy-q1*gz+q3*gx)*halfT;
    q3 = q3+(q0*gz+q1*gy-q2*gx)*halfT;

    //��һ��
    norm = sqrt(q0*q0+q1*q1+q2*q2+q3*q3);

    Q_info.q0 = q0/norm;
    Q_info.q1 = q1/norm;
    Q_info.q2 = q2/norm;
    Q_info.q3 = q3/norm;
}
/***********************************************
* @brief : ����Ԫ�������ŷ����
* @param : void
* @return: void
* @date  : 2023.2.22
* @author: L
************************************************/
void QuaterToEulerianAngles(void)
{
    ICMGetValue(value);
    ICMUpdate(value[3], value[4], value[5], value[0], value[1], value[2]);

    //��Ԫ��
    float q0 = Q_info.q0;
    float q1 = Q_info.q1;
    float q2 = Q_info.q2;
    float q3 = Q_info.q3;

    eulerangle.pitch = asin(-2*q1*q3+2*q0*q2)*180/PI;
    eulerangle.roll = atan2(2*q2*q3+2*q0*q1,-2*q1*q1-2*q2*q2+1)*180/PI;
    eulerangle.yaw = atan2(2*q1*q2+2*q0*q3,-2*q2*q2-2*q3*q3+1)*180/PI;
}
/***********************************************
* @brief : ��ȡICM20602Z����ٶ�
* @param : void
* @return: gyro_z:Z����ٶ�
* @date  : 2023.2.21
* @author: L
************************************************/
int16 GetICM20602Gyro_Z(void)
{
    uint8 dat[2];
    int16 gyro_z;

    ICM20602_CS(0);
    spi_read_8bit_registers(SPI_0, ICM20602_GYRO_ZOUT_H | ICM20602_SPI_R, dat, 2);
    ICM20602_CS(1);

    gyro_z = (int16)(((uint16)dat[0]<<8)|dat[1]);

    return gyro_z;
}

