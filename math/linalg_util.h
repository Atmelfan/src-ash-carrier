//
// Created by atmelfan on 2018-03-24.
//

#ifndef SRC_CARRIER_LINALG_UTIL_H
#define SRC_CARRIER_LINALG_UTIL_H

#include <stdbool.h>
#include "linalg.h"

/**
 *
 * @param rot
 * @param tra
 * @param mat
 */
void transform_matrix(const vec4* rot, const vec4* tra, mat4* mat);

/**
 * Check if a transform matrix is valid
 * @param mat, matrix to check
 * @return true if 0 <= m[1..3][1..3] <= 1, m44 = 1, and m4[1..3] = 0. False otherwise.
 */
bool transform_valid(const mat4* mat);

void mat4_rotx(float angle, mat4* mat);
void mat4_roty(float angle, mat4* mat);
void mat4_rotz(float angle, mat4* mat);
void mat4_trans(vec4* t, mat4* mat);


#endif //SRC_CARRIER_LINALG_UTIL_H
