//
// Created by atmelfan on 2018-03-20.
//

#include "gait.h"
#include "../leg.h"
#include "../fdt/dtb_parser.h"
#include "../platforms/log.h"
#include "../fdt_mathutil.h"

bool gait_update_step(gait_step *step, fdt_header_t *fdt, float t) {

    return 0;
}

void gait_init_target(gait_target *t) {
    t->initial = VEC4_ZERO();
    t->target = VEC4_ZERO();
}

void gait_step_begin(gait_step *step, gait_target *ts, fdt_header_t *fdt, fdt_token *node, uint32_t num) {


}
