//
// Created by atmelfan on 2018-03-24.
//

#ifndef SRC_CARRIER_LINALG_UTIL_H
#define SRC_CARRIER_LINALG_UTIL_H

#include <stdbool.h>
#include "linalg.h"

/**
 * Check if a transform matrix is valid
 * @param mat, matrix to check
 * @return true if 0 <= m[1..3][1..3] <= 1, m44 = 1, and m4[1..3] = 0. False otherwise.
 */
bool transform_valid(const mat4* mat);

/**
 * Make a matrix into a rotation matrix
 * @param angle, angle to rotate with (in radians)
 * @param mat, matrix to make into translation matrix
 * @return mat
 */
mat4 * mat4_make_rotx(float angle, mat4 *mat);
mat4 * mat4_make_roty(float angle, mat4 *mat);
mat4 * mat4_make_rotz(float angle, mat4 *mat);

/**
 * Make a matrix into a translation matrix
 * @param t, vector to translate with
 * @param mat, matrix to make into translation matrix
 * @return mat
 */
mat4 * mat4_make_translate(const vec4 *t, mat4 *mat);


#endif //SRC_CARRIER_LINALG_UTIL_H
