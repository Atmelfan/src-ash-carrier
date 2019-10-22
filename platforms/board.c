//
// Created by atmelfan on 2018-03-24.
//

#include <stdio.h>
#include "board.h"
#include "dev.h"
#include "log.h"

/**
 *
 */

void board_init_fdt(fdt_header_t *fdt, fdt_token *root) {
    dev_init_drivers();

    logd_push("board");
    dev_probe_devices_in_fdt(fdt, root, NULL);
    logd_pop();
}
