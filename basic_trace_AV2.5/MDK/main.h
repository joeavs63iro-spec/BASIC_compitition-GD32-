#ifndef MAIN_H
#define MAIN_H

#include "headfile.h"
#include "xunxian.h"

typedef enum {
    BRIDGE_NONE = 0,
    BRIDGE_KEEP,
} BridgeState_t;

typedef enum {
    PARKING_NONE = 0,
    PARKING_WAIT,
    PARKING_IN,
    PARKING_KEEP,
    PARKING_OUT,
} ParkingState_t;

typedef enum
{
    ROUND_NONE = 0,
    ROUND_IN,
    ROUND_KEEP,
    ROUND_OUT
} RoundState_t;

extern volatile PID_struct track_PID;

#endif