#ifndef CANMESSAGE_H
#define CANMESSAGE_H

#include <stdint.h>

#define CANMESSAGE_MAX_SIZE 16
#define CANMESSAGE_ID_SIZE sizeof(uint8_t)

//Test message
#define CANMESSAGE_ID_TEST 1
#define CANMESSAGE_ID_TEST_MSG_SIZE sizeof(float)

//Target Velocity (floats)
#define CANMESSAGE_ID_TARGET_VX 2
#define CANMESSAGE_ID_TARGET_VY 3
#define CANMESSAGE_ID_TARGET_VW 4
#define CANMESSAGE_ID_TARGET_READY 5

#define CANMESSAGE_ID_MOTOR_RPM 6



typedef struct CANMessage {
    uint8_t messageId;
    void* data;
} CANMessage;

#endif
