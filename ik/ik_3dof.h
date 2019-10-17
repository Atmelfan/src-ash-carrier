//
// Created by atmelfan on 3/29/18.
//

#ifndef SRC_CARRIER_IK_3DOF_H
#define SRC_CARRIER_IK_3DOF_H

#include "../math/linalg.h"
#include "../platforms/dev.h"

typedef struct {
    vec3 length;
    device_t pwm_dev;
    uint8_t servos[3];
} ik_3dof_data;






#endif //SRC_CARRIER_IK_3DOF_H
