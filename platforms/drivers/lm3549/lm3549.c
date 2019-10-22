//
// Created by atmelfan on 2018-03-24.
//

#include <stdlib.h>
#include <libopencm3/stm32/i2c.h>
#include <stdio.h>
#include "lm3549.h"
#include "../../dev.h"
#include "../../log.h"
#include "../../pwm.h"
#include "../../i2c.h"
#include "../../../fdt/fdt_parser.h"




static pwm_driver_t lm3549_drv;

DEV_PROBE_CALLBACK(lm3549, fdt, node, parent){
    device_t* parent_dev = parent;

    /* Check if parent exist and of I2C type */
    if(!parent_dev || parent_dev->type != DEV_TYPE_I2C){
        logd_printfs(LOG_ERROR, "parent must be an i2c controller!\n");
        return NULL;
    }

    lm3549_dev_t* lm3549 = malloc(sizeof(lm3549_dev_t));

    /*Generic device info*/
    if(!dev_init_from_fdt(&lm3549->dev.dev, fdt, node, DEV_TYPE_PWM)){
        logd_printfs(LOG_ERROR, "failed to init device_t\n");
        free(lm3549);
        return NULL;
    }
    lm3549->i2c = (i2c_dev_t *) parent;
    DEVICE_CAST(lm3549, device_t*)->drv = (driver_t *) &lm3549_drv;
    DEVICE_CAST(lm3549, device_t*)->reg &= 0xFE;


    /* Register so that others may find it */
    dev_register_device((device_t *) lm3549);

    logd_printf(LOG_INFO, "initialized %s!\n", DEVICE_CAST(lm3549, device_t*)->name);

    return (device_t *) lm3549;
}

/* Compatibility strings*/
static char* lm3549_compat[] = {
        "ti,lm3549",
        NULL
};

static pwm_driver_t lm3549_drv = {
        /* Implement super */
        DRIVER_SUPER(dev_driver, drv) = {
                .compatible = lm3549_compat,
                .probe = lm3549_probe,
                .close = NULL
        },
        .num_channels = 3,
        .period = 0,
        .set_pwm = NULL

};

void reg_lm3549_(){
    dev_register_driver((driver_t*)&lm3549_drv);
}
