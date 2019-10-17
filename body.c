//
// Created by atmelfan on 2018-04-15.
//

#include "body.h"

void body_reset(body_t* body){

}

void body_commit(body_t* body){
}

void body_rotate(body_t* body, vec4* rot){

}

void body_translate(body_t* body, vec4* tra){

}

void body_init(body_t* body){
    body->model = MAT4_IDENT();
    body->offset = VEC4_ZERO();
    body_reset(body);
}
