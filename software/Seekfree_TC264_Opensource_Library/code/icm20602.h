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

void GyroOffsetInit(void);
void ICMGetValue(float* value);
void ICMUpdate(float gx,float gy,float gz,float ax,float ay,float az);
void QuaterToEulerianAngles(void);
int16 GetICM20602Gyro_Z(void);

#endif /* CODE_ICM20602_H_ */
