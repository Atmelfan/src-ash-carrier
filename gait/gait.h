//
// Created by atmelfan on 2018-03-20.
//

#ifndef SRC_CARRIER_GAIT_H
#define SRC_CARRIER_GAIT_H

#include "../fdt/fdt_parser.h"
#include "../leg.h"
#include "gait.dtsi"


typedef struct  {
    bool raise[8];
} gait_step;

typedef enum {
    GAIT_IDLE = 0,  /* Not executing any gait code */
    GAIT_RUNNING,   /* Running gait code */
    GAIT_ENDNOW,    /* Next update will home */
    GAIT_FINAL      /* Currently homing */
} gait_state;

typedef struct  {
    gait_state state;

    /* Movement vector and phi */
    vec4 vec;
    float phi;

    /* Gait definition */
    uint32_t exit_phandle;
    uint32_t next_phandle;

} gait_t;

typedef struct {
    vec4 initial, target;
    float iangle, tangle;
    bool raise;
} gait_target;

#endif //SRC_CARRIER_GAIT_H
