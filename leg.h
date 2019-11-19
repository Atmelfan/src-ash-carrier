//
// Created by atmelfan on 2017-09-25.
//

#ifndef SRC_CARRIER_LEG_H
#define SRC_CARRIER_LEG_H

#include <stdbool.h>
#include "linalg/linalg.h"
#include "fdt/dtb_parser.h"
#include "platforms/pwm.h"

typedef enum {
    LEG_AT_HOME = 0,
    LEG_INTERPOLATING,
    LEG_AT_TARGET
} leg_status;


typedef struct {
    /* Applies translation/mirroring to convert an absolute pos to a leg relative pos. */
    mat4 transform;

    /* Home positioning */
    vec4 home_position;//Default position with maximum movement range
    vec4 offset_position;//Offset from default position

    /* Servo offsets from IK in degrees*100 */
    int32_t servo_offsets_100[3];
    uint32_t servo_index[3];
    int32_t lengths[3];
    uint8_t invert[3];
    uint32_t scale;

    /* PWM device used to control the servos */
    pwm_dev_t* pwm_dev;

    /* Status of leg movement */
    leg_status status;

    /* Leg is ready to be used */
    bool ready;
} leg_t;

/**
 * Initializes a leg with basic parameters
 * @param l, leg to initialize
 * @param scale, pulse width required for a 90 deg rotation in us
 */
void leg_init(leg_t *l, uint32_t scale);

/**
 * Reads leg parameters from fdt node
 * @param l, leg struct
 * @param fdt, fdt header
 * @param node, fdt node to read from
 * @return true if leg fdt is valid, false otherwise
 */
bool leg_from_node(leg_t* l, fdt_header_t* fdt, fdt_token* node);

/**
 * Move leg to local coordinates without interpolation
 * @param l, leg to move
 * @param loc, local coordinates in mm
 */
void leg_move_to_local(leg_t* l, vec4* loc);

/**
 * Move leg to global coordinates without interpolation
 * @param l, leg to move
 * @param vec, global coordinates in mm
 */
void leg_move_to_vec(leg_t* l, vec4* vec);



#endif //SRC_CARRIER_LEG_H
