#include "mecanisim_task.h"
#include <remote_control.h>
#include <stdlib.h>
#include "CAN_receive.h"
#include "PositionPID.h"
#include "arm_math.h"
#include "cmsis_os.h"
#include "stm32f4xx.h"

#define C610ANGLETOCODES 819.9f

PositionPIDData rightBallThingie;
PositionPIDData centerBallThingie;
PositionPIDData leftBallThingie;

void initMecanisimTask() {
  // Right ball thingie
  rightBallThingie.velocityPid = malloc(sizeof(arm_pid_instance_f32));

  rightBallThingie.velocityPid->Kp = 30.1;
  rightBallThingie.velocityPid->Ki = 0.0;
  rightBallThingie.velocityPid->Kd = 0.0;
  rightBallThingie.maximumVelocity = 10000;

  rightBallThingie.positionPid = malloc(sizeof(arm_pid_instance_f32));

  rightBallThingie.positionPid->Kp = 0.00085;
  rightBallThingie.positionPid->Ki = 0.0;
  rightBallThingie.positionPid->Kd = 0.0;
  rightBallThingie.positionLimitEnabled = false;

  // Center ball thingie
  centerBallThingie.velocityPid = malloc(sizeof(arm_pid_instance_f32));

  centerBallThingie.velocityPid->Kp = 30.1;
  centerBallThingie.velocityPid->Ki = 0.0;
  centerBallThingie.velocityPid->Kd = 0.0;
  centerBallThingie.maximumVelocity = 10000;

  centerBallThingie.positionPid = malloc(sizeof(arm_pid_instance_f32));

  centerBallThingie.positionPid->Kp = 0.00085;
  centerBallThingie.positionPid->Ki = 0.0;
  centerBallThingie.positionPid->Kd = 0.0;
  centerBallThingie.positionLimitEnabled = false;

  // Left ball thingie
  leftBallThingie.velocityPid = malloc(sizeof(arm_pid_instance_f32));

  leftBallThingie.velocityPid->Kp = 30.1;
  leftBallThingie.velocityPid->Ki = 0.0;
  leftBallThingie.velocityPid->Kd = 0.0;
  leftBallThingie.maximumVelocity = 10000;

  leftBallThingie.positionPid = malloc(sizeof(arm_pid_instance_f32));

  leftBallThingie.positionPid->Kp = 0.00085;
  leftBallThingie.positionPid->Ki = 0.0;
  leftBallThingie.positionPid->Kd = 0.0;
  leftBallThingie.positionLimitEnabled = false;

  initializePositionPid(&rightBallThingie);
  initializePositionPid(&centerBallThingie);
  initializePositionPid(&leftBallThingie);
}

extern motor_measure_t motor_chassis[8];

float rightSetPoint = 0.0f;
float centerSetPoint = 0.0f;
float leftSetPoint = 0.0f;

void mecanisimTaskUpdate(void *arguments) {
	for(;;){
		int16_t motor0Set = calculatePositionPid(
				&rightBallThingie, (float32_t)motor_chassis[0].speed_rpm / 36.0f,
				(float32_t)motor_chassis[0].total_ecd, rightSetPoint * C610ANGLETOCODES);

		int16_t motor1Set = calculatePositionPid(
				&leftBallThingie, (float32_t)motor_chassis[1].speed_rpm / 36.0f,
				(float32_t)motor_chassis[1].total_ecd, leftSetPoint * C610ANGLETOCODES);

		int16_t motor2Set = calculatePositionPid(
				&centerBallThingie, (float32_t)motor_chassis[2].speed_rpm / 36.0f,
				(float32_t)motor_chassis[2].total_ecd, centerSetPoint * C610ANGLETOCODES);

		CAN_CMD_CHASSIS(motor0Set, motor1Set, motor2Set, 0);

		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}
