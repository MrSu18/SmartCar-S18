/*
 * icm20602.h
 *
 *  Created on: 2023Äê2ÔÂ21ÈÕ
 *      Author: L
 */

#ifndef CODE_ICM20602_H_
#define CODE_ICM20602_H_

#include "zf_common_headfile.h"

#define PI 3.14
#define ICM_GET_Z
#define ICM_GET_Y
#define ICM_GET_X
typedef struct
{
    float q0;
    float q1;
    float q2;
    float q3;
}QuaterInfo;

typedef struct
{
    float pitch;
    float roll;
    float yaw;
}EulerAngle;

typedef struct
{
    int16 x;
    int16 y;
    int16 z;
}GyroOffset;

extern EulerAngle eulerangle;
extern float value[6];
extern int counter2ms;
extern int16 my_gyro_z;
void GyroOffsetInit(void);
void ICMGetValue(float* value);
void ICMUpdate(float gx,float gy,float gz,float ax,float ay,float az);
void QuaterToEulerianAngles(void);

#ifdef ICM_GET_Z
extern float my_angle_z;
extern float icm_target_angle_z;
extern uint8 icm_angle_z_flag;
int16 GetICM20602Gyro_Z(void);
void StartIntegralAngle_Z(float target_angle);
float GetICM20602Angle_Z(uint8 flag);
#endif

#ifdef ICM_GET_Y
extern float my_angle_y;
extern float icm_target_angle_y;
extern uint8 icm_angle_y_flag;
int16 GetICM20602Gyro_Y(void);
void StartIntegralAngle_Y(float target_angle);
float GetICM20602Angle_Y(uint8 flag);
#endif

#ifdef ICM_GET_X
extern float my_angle_x;
extern float icm_target_angle_x;
extern uint8 icm_angle_x_flag;
int16 GetICM20602Gyro_X(void);
void StartIntegralAngle_X(float target_angle);
float GetICM20602Angle_X(uint8 flag);
#endif

#endif /* CODE_ICM20602_H_ */
