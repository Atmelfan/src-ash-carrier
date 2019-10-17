//
// Created by atmelfan on 4/15/18.
//

#ifndef SRC_CARRIER_FDT_MATHUTIL_H
#define SRC_CARRIER_FDT_MATHUTIL_H

#include <stdbool.h>
#include "fdt/dtb_parser.h"
#include "math/linalg.h"

bool fdtmath_node_get_vec(fdt_header_t* fdt, fdt_token* t, vecx* vec);

bool fdtmath_node_get_mat(fdt_header_t* fdt, fdt_token* t, matxx* mat);

#endif //SRC_CARRIER_FDT_MATHUTIL_H
