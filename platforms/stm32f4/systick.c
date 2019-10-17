//
// Created by atmelfan on 4/15/18.
//
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/rcc.h>
#include "../systick.h"
#include "../log.h"

static systick_drv_t stm32f4_systick_drv;

DEV_PROBE_CALLBACK(stm32f4_systick, fdt, node, parent){
    systick_dev_t* systick = malloc(sizeof(systick_dev_t));

    /*Generic device info*/
    if(!dev_init_from_fdt(&systick->dev, fdt, node, DEV_TYPE_I2C)){
        logd_printfs(LOG_ERROR, "failed to init device_t\n");
        free(systick);
        return NULL;
    }

    /* Add driver info */
    systick->dev.drv = (driver_t*)&stm32f4_systick_drv;


    /* Get frequency (default = 100KHz) */
    uint32_t freq = fdt_node_get_u32(fdt, node, "freq", 100000);
    systick_set_reload(rcc_ahb_frequency/freq);
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
    systick_counter_enable();
    systick_interrupt_enable();


    /* Register device if successfull */
    dev_register_device(&systick->dev);
    log_printf(&default_log, LOG_INFO, "Initialized %s!\n", systick->dev.name);

    return (device_t *) systick;

}

DEV_COMPAT_LIST(stm32f4_systick) = {
        "stm32f4,systick",
        NULL
};

static systick_drv_t stm32f4_systick_drv = {
        /* Implement super */
        DRIVER_SUPER(driver_t, drv) = {
                .compatible = stm32f4_systick_compat,
                .probe = stm32f4_systick_probe,
                .close = NULL
        },

};

DEV_REGISTER_CALLBACK(stm32f4_systick){
    dev_register_driver((driver_t *) &stm32f4_systick_drv);
}