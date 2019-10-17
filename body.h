//
// Created by atmelfan on 2018-04-15.
//

#ifndef SRC_CARRIER_BODY_H
#define SRC_CARRIER_BODY_H

#include "math/linalg.h"

typedef struct {
    vec4 offset;
    mat4 model;
} body_t;

void body_init(body_t* body);

void body_rotate(body_t* body, vec4* rot);

void body_translate(body_t* body, vec4* tra);

void body_reset(body_t* body);

void body_commit(body_t* body);

/*





*/

#endif //SRC_CARRIER_BODY_H
