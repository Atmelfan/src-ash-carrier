//
// Created by atmelfan on 2017-09-20.
//

#ifndef SRC_CARRIER_LINALG_H
#define SRC_CARRIER_LINALG_H

#include <stdint.h>

#define LVEC_VARIANT(name, namel) static inline void namel (vecx* l, const vecx* r) {name(l, r, l);}
#define LMAT_VARIANT(name, namel) static inline void namel (matxx* l, const matxx* r) {name(l, r, l);}

/*Sqrt function to be used*/
#define LINALG_SQRT_F(x) (x)

typedef float mat_f;
typedef int16_t mat_i;

/************************************************FLOATING POINT MATRICES***********************************************/
typedef struct {
    uint8_t w, h;
    mat_f members[0];
} matxx;

typedef struct {
    matxx mat;
    mat_f members[9];
} mat3;

#define MAT3(...) {{3,3}, {__VA_ARGS__}}
#define MAT3_ZERO() {{3,3}, {0}}
#define MAT3_M(x,y) member[(y)*3 + (x)]

typedef struct {
    matxx mat;
    mat_f members[16];
} mat4;

#define MAT4(...) {{4,4}, {__VA_ARGS__}}
#define MAT4_ZERO() (mat4){{4,4}, {0}}
#define MAT4_IDENT() (mat4){{4,4}, {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, }}
#define MAT4_M(x,y) members[(y)*4 + (x)]

/**MOVES CONTENT OF r INTO l**/
void mat_mov(matxx* l, const matxx* r);

/**ADDITION**/
void mat_add(const matxx* l, const matxx* r, matxx* o);
LMAT_VARIANT(mat_add, mat_addl)

/**SUBTRACTION**/
void mat_sub(const matxx* l, const matxx* r, matxx* o);
LMAT_VARIANT(mat_sub, mat_subl)

/**MULTIPLICATION**/
void mat_mul(const matxx* l, const matxx* r, matxx* o);
LMAT_VARIANT(mat_mul, mat_mull)

/**DIVISION**/
void mat_div(const matxx* l, const matxx* r, matxx* o);
LMAT_VARIANT(mat_div, mat_divl)

/**MATRIX MULTIPLICATION**/
void mat_matmul(const matxx* l, const matxx* r, matxx* o);
LMAT_VARIANT(mat_matmul, mat_matmull)/*Only works for cubic matrices!*/

/************************************************FLOATING POINT VECTORS************************************************/
typedef struct {
    uint8_t l;
    mat_f members[0];
} vecx;

typedef struct {
    vecx vecx;
    mat_f members[3];
} vec3;

#define VEC3(...) {{3}, {__VA_ARGS__}}
#define VEC3_ZERO() (vec3){{3}, {0}}

typedef struct {
    vecx vecx;
    mat_f members[4];
} vec4;

#define VEC4(...) {{4}, {__VA_ARGS__}}
#define VEC4_ZERO() (vec4){{4}, {0}}

/**MOVES CONTENT OF r INTO l**/
void vec_mov(vecx* l, const vecx* r);
void vec_movx(vecx* l, const vecx* r, uint8_t s, uint8_t e);//Moves only (e - s) number of members starting at s

/**ADDITION**/
void vec_add(const vecx* l, const vecx* r, vecx* o);
LVEC_VARIANT(vec_add, vec_addl)

/**SUBTRACTION**/
void vec_sub(const vecx* l, const vecx* r, vecx* o);
LVEC_VARIANT(vec_sub, vec_subl)

/**MULTIPLICATION**/
void vec_mul(const vecx* l, const vecx* r, vecx* o);
LVEC_VARIANT(vec_mul, vec_mull)

/**DIVISION**/
void vec_div(const vecx* l, const vecx* r, vecx* o);
LVEC_VARIANT(vec_div, vec_divl)

/**CROSS PRODUCT**/
void vec_cross(const vecx* l, const vecx* r, vecx* o);
//LVEC_VARIANT(vec_cross, vec_crossl)

/**SCALING**/
void vec_scale(const vecx* l, mat_f s, vecx* o);
static inline void vec_scalel(vecx* l, mat_f s) {vec_scale(l, s, l);}

/**NORMALIZATION**/
void vec_normal(const vecx* l, vecx* o);
static inline void vec_normall(vecx* l) {vec_normal(l, l);}

/**DOT PRODUCT**/
mat_f vec_dot(const vecx* l, const vecx* r);

/**LENGTH**/
mat_f vec_length(const vecx* l);
mat_f vec_lengthsq(const vecx* l);

/***********************************************MATRIX-VECTOR OPERATIONS***********************************************/

void vecmat_mul(const matxx* r, const vecx* l, vecx* o);

#endif //SRC_CARRIER_LINALG_H
