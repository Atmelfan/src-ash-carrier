//
// Created by atmelfan on 2018-03-21.
//

#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/rcc.h>
#include <assert.h>
#include <stdio.h>
#include "../i2c.h"
#include "../dev.h"
#include "../../fdt/dtb_parser.h"
#include "../log.h"

#define I2C_MAX_FREQ 400000


uint32_t stm32f4_i2c_tx(const i2c_dev_t* dev, uint8_t* tx, uint32_t len);

static i2c_drv_t stm32f4_i2c_drv;

bool stm32f4_i2c_init(i2c_dev_t* i2c, uint32_t speed){
    if(i2c->dev.type != DEV_TYPE_I2C)
        return false;

    /*Check and enable clock*/
    enum rcc_periph_clken clken;
    switch(i2c->dev.reg){
        case I2C1:
            clken = RCC_I2C1;
            break;
#ifdef I2C2
        case I2C2:
            clken = RCC_I2C2;
            break;
#endif
#ifdef I2C3
        case I2C3:
            clken = RCC_I2C3;
            break;
#endif
        default:
            return false;
    }
    rcc_periph_clock_enable(clken);


    /* Calculate clock prescaler */
    uint32_t i2c_freq_mhz = rcc_apb1_frequency / 1000000;
    uint32_t ccr = rcc_apb1_frequency / speed / 2 + 1;
    if (ccr < 4)
        ccr = 4;

    /*Reset I2C*/
    i2c_reset(i2c->dev.reg);
    i2c_peripheral_disable(i2c->dev.reg);

    /*Set clock registers*/
    i2c_set_clock_frequency(i2c->dev.reg, i2c_freq_mhz);
    i2c_set_trise(i2c->dev.reg, i2c_freq_mhz + 1);
    i2c_set_ccr(i2c->dev.reg, ccr);

    /*Enable*/
    I2C_CR1(i2c->dev.reg) = 0;
    I2C_OAR1(i2c->dev.reg) = 0x00000000;
    i2c_peripheral_enable(i2c->dev.reg);
    for (int i = 0; i < 10000000U; ++i) {
        asm("nop");
    }


    return true;
}

uint32_t stm32f4_i2c_tx(const i2c_dev_t* dev, uint8_t* tx, uint32_t len){
    /**/
    for (int i = 0; i < len; ++i) {
        i2c_send_data(dev->dev.reg, tx[i]);
        while (!(I2C_SR1(dev->dev.reg) & I2C_SR1_BTF));
    }

    return len;
}

uint32_t stm32f4_i2c_rx(const i2c_dev_t* dev, uint8_t* rx, uint32_t len){

    /**/
    for (int i = 0; i < len; ++i) {
        while (!(I2C_SR1(dev->dev.reg) & I2C_SR1_BTF));
        rx[i] = i2c_get_data(dev->dev.reg);
    }
    return len;
}

uint32_t stm32f4_i2c_start(const i2c_dev_t* dev, uint8_t addr){
    /* Send start condition */
    i2c_send_start(dev->dev.reg);
    while (!((I2C_SR1(dev->dev.reg) & I2C_SR1_SB) & (I2C_SR2(dev->dev.reg) & (I2C_SR2_MSL | I2C_SR2_BUSY))));

    /* Send slave address */
    i2c_send_7bit_address(dev->dev.reg, (uint8_t)(addr  >> 1), I2C_WRITE);
    while (!(I2C_SR1(dev->dev.reg) & I2C_SR1_ADDR));

    uint32_t tmp = I2C_SR2(dev->dev.reg);
    return 0;
}

uint32_t stm32f4_i2c_stop(const i2c_dev_t* dev){
    i2c_send_stop(dev->dev.reg);
    return 0;
}


DEV_PROBE_CALLBACK(stm32f4_i2c, fdt, node, parent){

    i2c_dev_t* i2c = malloc(sizeof(i2c_dev_t));

    /*Generic device info*/
    if(!dev_init_from_fdt(&i2c->dev, fdt, node, DEV_TYPE_I2C)){
        logd_printfs(LOG_ERROR, "failed to init device_t\n");
        free(i2c);
        return NULL;
    }

    /* Add driver info */
    i2c->dev.drv = (driver_t*)&stm32f4_i2c_drv;

    /* Get own address */
    uint32_t addr = fdt_node_get_u32(fdt, node, "addr", 0x00);
    if(addr > UINT8_MAX){
        logd_printfs(LOG_WARNING, "address out of range\n");
    }
    i2c->own_address = (uint8_t)(addr & 0xFE);

    /* Get speed (default = 100KHz) */
    uint32_t speed = fdt_node_get_u32(fdt, node, "speed", 100000);
    if(speed > I2C_MAX_FREQ){
        logd_printfs(LOG_WARNING, "speed out of range\n");
    }
    i2c->speed = speed;

    /*TODO: Fix GPIO API & PINS!*/

    /* Init peripheral */
    if(!stm32f4_i2c_init(i2c, i2c->speed)){
        logd_printfs(LOG_ERROR, "failed to init I2C peripheral\n");
        free(i2c);
        return NULL;
    }
    //i2c_write_reg(i2c, 0x8E, 0, NULL, 6);

    /* Register device if successfull */
    dev_register_device(&i2c->dev);
    log_printf(&default_log, LOG_INFO, "Initialized %s!\n", i2c->dev.name);

    return (device_t *) i2c;
}

/* List of compatibility strings */
DEV_COMPAT_LIST(stm32f4_i2c) = {
        "stm32f4,i2c-ctr",
        DEV_COMPAT_LIST_END
};

/**
 * Driver struct extends the base driver_t
 */
static i2c_drv_t stm32f4_i2c_drv = {
    /* Implement super */
    DRIVER_SUPER(driver_t, drv) = {
        .compatible = stm32f4_i2c_compat,
        .probe = stm32f4_i2c_probe,
        .close = NULL
    },
    /* Transmit/Receive functions */
    .rx = stm32f4_i2c_rx,
    .tx = stm32f4_i2c_tx,
    .start = stm32f4_i2c_start,
    .stop = stm32f4_i2c_stop
};



DEV_REGISTER_CALLBACK(stm32f4_i2c){
    dev_register_driver((driver_t*)&stm32f4_i2c_drv);
}



