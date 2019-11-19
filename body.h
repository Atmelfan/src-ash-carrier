//
// Created by atmelfan on 2018-04-15.
//

#ifndef SRC_CARRIER_BODY_H
#define SRC_CARRIER_BODY_H

#include "linalg/linalg.h"

typedef struct {
    /* Body model matrix */
    mat4 model;
} body_t;

/**
 * Initialize body
 * @param body
 */
void body_init(body_t* body);

/**
 * Rotate body according to vector
 * @param body, body to be rotated
 * @param rot, angles to rotate with (ROLL, PITCH, YAW, _)
 * @return body
 */
body_t * body_rotate(body_t *body, const vec4 *rot);

/**
 * Translate body according to vector
 * @param body, body to be translated
 * @param tra, translation vector
 * @return body
 */
body_t * body_translate(body_t *body, const vec4 *tra);

/**
 * Reset body to zero translation and rotation
 * @param body
 */
void body_reset(body_t* body);

/**
 * Apply body model matrix to a point
 * @param body, body to be applies
 * @param point, point to be transformed
 */
vec4 * body_apply(body_t *body, vec4 *point);


void body_commit(body_t* body);

#endif //SRC_CARRIER_BODY_H
