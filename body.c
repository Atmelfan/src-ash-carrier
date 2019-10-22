//
// Created by atmelfan on 2018-04-15.
//

#include "body.h"
#include "math/linalg_util.h"

void body_reset(body_t* body){
    body->model = MAT4_IDENT();
}


body_t * body_rotate(body_t *body, const vec4 *rot){
    mat4 tmp;

    /* Apply rotation */
    mat_matmull((matxx *) &body->model, (matxx *) mat4_make_rotx(rot->members[0], &tmp));// Rotate around X (ROLL)
    mat_matmull((matxx *) &body->model, (matxx *) mat4_make_roty(rot->members[1], &tmp));// Rotate around Y (PITCH)
    mat_matmull((matxx *) &body->model, (matxx *) mat4_make_rotz(rot->members[2], &tmp));// Rotate around Z (YAW)

    /* Return body */
    return body;
}

body_t * body_translate(body_t *body, const vec4 *tra){
    mat4 tmp;

    /* Apply translation */
    mat_matmull((matxx *) &body->model, (const matxx *) mat4_make_translate(tra, &tmp));

    /* Return body */
    return body;
}

void body_init(body_t* body){
    body_reset(body);
}

vec4* body_apply(body_t *body, vec4 *point) {
    vec4 tmp = *point;

    /* Multiply the body model matrix with point (transforming it) */
    vecmat_mul((matxx*) &body->model, (vecx*) &tmp, (vecx*) &point);

    /* Return transformed point */
    return point;
}
