//
// Created by atmelfan on 2017-09-20.
//
#include <assert.h>
#include "linalg.h"

/**MOVES CONTENT OF r INTO l**/
void mat_mov(matxx* l, const matxx* r){
    //const uint8_t W = l->w;
    const uint8_t t = l->w*l->h;
    for(int i = 0; i < t; ++i){
        l->members[i] = r->members[i];
    }
}

/**ADDITION**/
void mat_add(const matxx* l, const matxx* r, matxx* o){
    const uint8_t t = l->w*l->h;
    for(int i = 0; i < t; ++i){
        o->members[i] = l->members[i] + r->members[i];
    }
}


/**SUBTRACTION**/
void mat_sub(const matxx* l, const matxx* r, matxx* o){
    const uint8_t t = l->w*l->h;
    for(int i = 0; i < t; ++i){
        o->members[i] = l->members[i] - r->members[i];
    }
}


/**MULTIPLICATION**/
void mat_mul(const matxx* l, const matxx* r, matxx* o){
    const uint8_t t = l->w*l->h;
    for(int i = 0; i < t; ++i){
        o->members[i] = l->members[i] * r->members[i];
    }
}


/**DIVISION**/
void mat_div(const matxx* l, const matxx* r, matxx* o){
    const uint8_t t = l->w*l->h;
    for(int i = 0; i < t; ++i){
        o->members[i] = l->members[i] / r->members[i];
    }
}

/**MATRIX MULTIPLICATION**/
void mat_matmul(const matxx* l, const matxx* r, matxx* o){
    const uint8_t HL = l->h;
    const uint8_t WR = r->w;
    const uint8_t WL = l->w;
    assert(l->w == r->h && "Matrix dimensions must agree!");
    assert(o->w == WR && o->h == HL && "Output matrix dimensions must agree!");
    for(int x = 0; x < WR; ++x){
        for(int y = 0; y < HL; ++y){
            mat_f mxy = 0;
            for(int j = 0; j < WL; ++j){
                mxy += l->members[y*WL + j]*r->members[j*WR + x];
            }
            o->members[y*WL + x] = mxy;
        }
    }
}
