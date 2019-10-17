//
// Created by atmelfan on 4/15/18.
//

#include "fdt_mathutil.h"
#include "ik/ik_3dof.h"

bool fdtmath_node_get_vec(fdt_header_t *fdt, fdt_token *t, vecx *vec) {
    if(!t)
        return false;

    /* Read all members or what's available*/
    for(int i = 0; i < vec->l && i < fdt_prop_len(fdt, t)/sizeof(uint32_t); ++i){
        vec->members[i] = ((int32_t)fdt_read_u32(&t->cells[i]))/1000.0f;
    }

    return true;
}

bool fdtmath_node_get_mat(fdt_header_t *fdt, fdt_token *t, matxx *mat) {
    if(!t)
        return false;

    /* Read all members or what's available*/
    for(int i = 0; i < mat->h*mat->w && i < fdt_prop_len(fdt, t)/sizeof(uint32_t); ++i){
        mat->members[i] = ((int32_t)fdt_read_u32(&t->cells[i]))/1000.0f;
    }

    return true;
}
