#include <assert.h>
#include "linalg.h"

//
// Created by atmelfan on 2017-09-21.
//
void vecmat_mul(const matxx* r, const vecx* l, vecx* o){
    uint8_t L = l->l;
    assert(L == r->h && L == r->w && "Matrix and vector dimensions must match!");
    for(int i = 0; i < L; ++i){
        mat_f sum = 0;
        for(int j = 0; j < L; ++j){
            sum += r->members[i*L + j]*l->members[j];
        }
        o->members[i] = sum;
    }
}
