#ifndef CAN_COM_H
#define CAN_COM_H

#include "structs.h"
#include "user_config.h"
#include "mbed.h"
#include "math_ops.h"

void pack_reply(CANMessage *msg, float p, float v, float t, float temperature);
void pack_Oreply(CANMessage *msg, float p, float v, float t, float temperature);
void unpack_cmd(CANMessage msg, ControllerStruct * controller);


#endif