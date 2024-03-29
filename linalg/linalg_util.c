//
// Created by atmelfan on 2018-03-24.
//

#include <math.h>
#include "linalg_util.h"
#include "linalg.h"

bool transform_valid(const mat4* mat){
    for(int i = 0; i < 9; ++i){
        float f = mat->members[i%3 + 3*(i/3)];
        if(fabs(f) > 1.0f){
            return false;
        }
    }

    return mat->members[15] == 1.0f
            && mat->members[14] == 0.0f
            && mat->members[13] == 0.0f
            && mat->members[12] == 0.0f;
}

mat4 * mat4_make_rotx(float angle, mat4 *mat){
    // Reset matrix
    *mat = MAT4_IDENT();

    // Fill a 3x3 rotation matrix
    float c = cosf(angle), s = sinf(angle);
    mat->MAT4_M(1,1) =  c;//m00
    mat->MAT4_M(2,1) = -s;//m01
    mat->MAT4_M(1,2) =  s;//m10
    mat->MAT4_M(2,2) =  c;//m11

    return mat;
}

mat4 * mat4_make_roty(float angle, mat4 *mat){
    // Reset matrix
    *mat = MAT4_IDENT();

    // Fill a 3x3 rotation matrix
    float c = cosf(angle), s = sinf(angle);
    mat->MAT4_M(0,0) =  c;//m00
    mat->MAT4_M(2,0) =  s;//m01
    mat->MAT4_M(0,2) = -s;//m10
    mat->MAT4_M(2,2) =  c;//m11

    return mat;
}

mat4 * mat4_make_rotz(float angle, mat4 *mat){
    // Reset matrix
    *mat = MAT4_IDENT();

    // Fill a 3x3 rotation matrix
    float c = cosf(angle), s = sinf(angle);
    mat->MAT4_M(0,0) =  c;//m00
    mat->MAT4_M(1,0) = -s;//m01
    mat->MAT4_M(0,1) =  s;//m10
    mat->MAT4_M(1,1) =  c;//m11

    return mat;
}

mat4 * mat4_make_translate(const vec4 *t, mat4 *mat){
    // Reset matrix
    *mat = MAT4_IDENT();

    // Fill column 4 with translation values
    mat->MAT4_M(3,0) = t->members[0];//m00
    mat->MAT4_M(3,1) = t->members[1];//m01
    mat->MAT4_M(3,2) = t->members[2];//m10

    return mat;
}






