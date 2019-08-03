#ifndef CHASSIS_CONTROL_H
#define CHASSIS_CONTROL_H
#include "struct_typedef.h"

//���̵���ٶȻ�PID
#define M3505_MOTOR_SPEED_PID_KP 12500.0f
#define M3505_MOTOR_SPEED_PID_KI 0.0f
#define M3505_MOTOR_SPEED_PID_KD 6.66f
#define M3505_MOTOR_SPEED_PID_MAX_OUT MAX_MOTOR_CAN_CURRENT
#define M3505_MOTOR_SPEED_PID_MAX_IOUT 2000.0f

//������ת����PID
#define CHASSIS_ROTATION_PID_KP 0.0f
#define CHASSIS_ROTATION_PID_KI 0.0f
#define CHASSIS_ROTATION_PID_KD 0.0f
#define CHASSIS_ROTATION_PID_MAX_OUT 0.0f
#define CHASSIS_ROTATION_PID_MAX_IOUT 0.0f

//������ת����PID
#define CHASSIS_ANGLE_PID_KP 45.0f
#define CHASSIS_ANGLE_PID_KI 0.0f
#define CHASSIS_ANGLE_PID_KD 1.0f
#define CHASSIS_ANGLE_PID_MAX_OUT 6.0f
#define CHASSIS_ANGLE_PID_MAX_IOUT 0.2f

#endif
