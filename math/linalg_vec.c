//
// Created by atmelfan on 2017-09-20.
//

#include <assert.h>
#include "linalg.h"

/**MOVES CONTENT OF r INTO l**/
void vec_mov(vecx* l, const vecx* r){
    assert(l->l == r->l && "Vector lengths must match!");
    for(int i = 0; i < l->l; ++i){
        l->members[i] = r->members[i];
    }
}

void vec_movx(vecx* l, const vecx* r, uint8_t s, uint8_t e){
    assert(l->l == r->l && "Vector lengths must match!");
    assert(s <= e && "End must be after start!");
    assert(e <= l->l && "End must be within vector!");
    for(int i = s; i < e; ++i){
        l->members[i] = r->members[i];
    }
}

/**ADDITION**/
void vec_add(const vecx* l, const vecx* r, vecx* o){
    assert(l->l == r->l && "Vector lengths must match!");
    for(int i = 0; i < l->l; ++i){
        o->members[i] = l->members[i] + r->members[i];
    }
}

/**SUBTRACTION**/
void vec_sub(const vecx* l, const vecx* r, vecx* o){
    assert(l->l == r->l && "Vector lengths must match!");
    for(int i = 0; i < l->l; ++i){
        o->members[i] = l->members[i] - r->members[i];
    }
}

/**MULTIPLICATION**/
void vec_mul(const vecx* l, const vecx* r, vecx* o){
    assert(l->l == r->l && "Vector lengths must match!");
    for(int i = 0; i < l->l; ++i){
        o->members[i] = l->members[i] * r->members[i];
    }
}

/**DIVISION**/
void vec_div(const vecx* l, const vecx* r, vecx* o){
    assert(l->l == r->l && "Vector lengths must match!");
    for(int i = 0; i < l->l; ++i){
        o->members[i] = l->members[i] / r->members[i];
    }
}

/**CROSS PRODUCT**/
void vec_cross(const vecx* l, const vecx* r, vecx* o){
    assert((l->l == r->l && l->l > 2) && "Vector lengths must match!");
    //Only 3 dimensions will be evaluated, additional will be ignored
    //Vectors of length less than 3 will assert false.
    o->members[0] = l->members[1]*r->members[2] - l->members[2]*r->members[1];
    o->members[1] = l->members[2]*r->members[0] - l->members[0]*r->members[2];
    o->members[2] = l->members[0]*r->members[1] - l->members[1]*r->members[0];
}

/**SCALING**/
void vec_scale(const vecx* l, mat_f s, vecx* o){
    for(int i = 0; i < l->l; ++i){
        o->members[i] = l->members[i]*s;
    }
}

/**NORMALIZATION**/
void vec_normal(const vecx* l, vecx* o){
    mat_f s = vec_length(l);
    vec_scale(l, s, o);
}

/**DOT PRODUCT**/
mat_f vec_dot(const vecx* l, const vecx* r){
    mat_f sum = 0;
    assert(l->l == r->l && "Vector lengths must match!");
    for(int i = 0; i < l->l; ++i){
        sum += l->members[i] * r->members[i];
    }
    return sum;
}

/**LENGTH**/
mat_f vec_length(const vecx* l){
    return LINALG_SQRT_F(vec_lengthsq(l));
}

mat_f vec_lengthsq(const vecx* l){
    mat_f sum = 0;
    for(int i = 0; i < l->l; ++i){
        mat_f t = l->members[i];
        sum += t*t;
    }
    return sum;
}
