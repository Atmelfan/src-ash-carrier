//
// Created by atmelfan on 2018-03-24.
//

#include <stdlib.h>
#include <libopencm3/stm32/i2c.h>
#include <stdio.h>
#include "pca9685.h"
#include "../../dev.h"
#include "../../log.h"
#include "../../pwm.h"
#include "../../i2c.h"
#include "../../../fdt/fdt_parser.h"

#define PCA9685_REG_MODE1 0x00
#define PCA9685_REG_MODE2 0x01
#define PCA9685_REG_PRESCALE 0xFE
#define PCA9685_REG_LED_ON_L 0xFA
#define PCA9685_REG_LED_ON_H 0xFB
#define PCA9685_REG_LED_OFF_L 0xFC
#define PCA9685_REG_LED_OFF_H 0xFD
#define PCA9685_REG_LEDn_L(n) (0x06 + ((n)*4))

#define PCA9685_PERIOD 4096
#define PCA9685_NUM_CH 16

#define DELAY for (int i = 0; i < 100000; ++i){asm("nop");}



pwm_status pca9685_set_pwm(pwm_dev_t* dev, uint16_t ch, uint32_t off, uint32_t on){

    /* Sanity checks */
    if(ch >= PCA9685_NUM_CH)
        return PWM_OUTACHANNELS;

    if(off + on > PCA9685_PERIOD)
        return PWM_OUTATIME;

    /* Set L/H bytes of specified channel*/
    uint8_t buf[4] = {
            (uint8_t)((off)),
            (uint8_t)((off >> 8)),
            (uint8_t)((on)),
            (uint8_t)((on >> 8))
    };
    i2c_write_reg(((pca9685_dev_t*)dev)->i2c, (uint8_t)((device_t*)dev)->reg, (uint8_t)(0x06 + 4*ch), buf, 4);

    return PWM_OK;
}

static pwm_driver_t pca9685_drv;

DEV_PROBE_CALLBACK(pca9685, fdt, node, parent){
    device_t* parent_dev = parent;

    /* Check if parent exist and of I2C type */
    if(!parent_dev || parent_dev->type != DEV_TYPE_I2C){
        logd_printfs(LOG_ERROR, "parent must be an i2c controller!\n");
        return NULL;
    }

    pca9685_dev_t* pca9685 = malloc(sizeof(pca9685_dev_t));



    /*Generic device info*/
    if(!dev_init_from_fdt(&pca9685->dev.dev, fdt, node, DEV_TYPE_PWM)){
        logd_printfs(LOG_ERROR, "failed to init device_t\n");
        free(pca9685);
        return NULL;
    }
    pca9685->i2c = (i2c_dev_t *) parent;
    pca9685->dev.dev.drv = (driver_t *) &pca9685_drv;

    /* Check address */
    uint8_t reg = (uint8_t)(DEVICE_CAST(pca9685, device_t*)->reg & 0xFE);


    /* Get external clock if any */
    uint32_t extclk = fdt_node_get_u32(fdt, node, "extclk", 0);

    /* Enable external clock if extclk != 0 and exist */
    bool extclk_en = true;
    if(!extclk){
        extclk = 25000000;
        extclk_en = false;
    }

    /* Get and calculate clock prescaler */
    uint32_t freq = fdt_node_get_u32(fdt, node, "freq", 0x00);
    uint32_t prescaler = (extclk / 4096 / freq) - 1;
    if(prescaler > UINT8_MAX || prescaler < 3){
        logd_printfs(LOG_WARNING, "frequency out of range\n");
    }


    uint8_t tmp;
    /* Set sleep mode + auto increment */
    tmp = 0x30;
    i2c_write_reg((const i2c_dev_t *) parent_dev, reg, PCA9685_REG_MODE1, &tmp, 1);

    /* Set prescaler */
    tmp = (uint8_t)prescaler;
    i2c_write_reg((i2c_dev_t*)parent_dev, reg, PCA9685_REG_PRESCALE, &tmp, 1);

    /* Get initial on/off values */
    fdt_token* all = fdt_node_get_prop(fdt, node, "all", false);
    if(all){
        if(fdt_prop_len(fdt, all) == 2*sizeof(uint32_t)){
            /* Set both all-off and all-on */
            uint16_t all_on = (uint16_t)(fdt_read_u32(&all->cells[0]) % 4096);
            uint16_t all_off = (uint16_t)(fdt_read_u32(&all->cells[1]) % 4096);
            uint8_t buf[] = {
                    (uint8_t)((all_on >> 0) & 0xFF),
                    (uint8_t)((all_on >> 8) & 0xFF),
                    (uint8_t)((all_off >> 0) & 0xFF),
                    (uint8_t)((all_off >> 8) & 0xFF)
            };
            i2c_write_reg((i2c_dev_t*)parent_dev, reg, PCA9685_REG_LED_ON_L , buf, 4);
        }else if(fdt_prop_len(fdt, all) == 1*sizeof(uint32_t)){
            /*Set only all on*/
            uint16_t all_on = (uint16_t)(fdt_read_u32(&all->cells[0]) % 4096);
            uint8_t buf[] = {
                    (uint8_t)((all_on >> 0) & 0xFF),
                    (uint8_t)((all_on >> 8) & 0xFF)
            };
            i2c_write_reg((i2c_dev_t*)parent_dev, reg, PCA9685_REG_LED_ON_L, buf, 2);
        }else{
            logd_printfs(LOG_WARNING, "expected 1-2 cells in 'all'\n");
        }
    }else{
        fdt_token* widths = fdt_node_get_prop(fdt, node, "widths", false);
        if(widths){
            /* Iterate all on/off pairs */
            //logd_printfs(LOG_INFO, "---\n");
            for (int i = 0; i < fdt_prop_len(fdt, widths)/(2*sizeof(uint32_t)) && i < PCA9685_NUM_CH; ++i) {
                pca9685_set_pwm((pwm_dev_t *) pca9685,
                                (uint16_t) i,
                                fdt_read_u32(&widths->cells[2 * i + 0]),
                                fdt_read_u32(&widths->cells[2 * i + 1]));
            }
        }else{
            logd_printfs(LOG_INFO, "no initialization values found!\n");
        }
    }

    /* Set output properties */


    bool opendrain = fdt_node_get_prop(fdt, node, "open-drain", false) ? true : false;
    bool invert = fdt_node_get_prop(fdt, node, "invert-output", false) ? true : false;
    bool oehighz = fdt_node_get_prop(fdt, node, "oe-highz", false) ? true : false;

    tmp = (uint8_t)(0x00 | (invert ? 0x10 : 0x00) | (opendrain ? 0x00 : 0x04) | (oehighz ? 0x02 : 0x00));
    i2c_write_reg((i2c_dev_t*)parent_dev, reg, PCA9685_REG_MODE2, &tmp, 1);
    for (int j = 0; j < 10000000; ++j) {
        asm("nop");
    }
    /*Restart + auto increment + extclk*/
    tmp = 0xA0;
    i2c_write_reg((i2c_dev_t*)parent_dev, reg, PCA9685_REG_MODE1, &tmp, 1);

    /* Register so that others may find it */
    dev_register_device((device_t *) pca9685);

    logd_printf(LOG_INFO, "initialized %s!\n", DEVICE_CAST(pca9685, device_t*)->name);

    return (device_t *) pca9685;
}

/* Compatibility strings*/
static char* pca9685_compat[] = {
        "nxp,pca9685",
        NULL
};

static pwm_driver_t pca9685_drv = {
        /* Implement super */
        DRIVER_SUPER(dev_driver, drv) = {
                .compatible = pca9685_compat,
                .probe = pca9685_probe,
                .close = NULL
        },
        .num_channels = PCA9685_NUM_CH,
        .period = PCA9685_PERIOD,
        .set_pwm = pca9685_set_pwm

};

void reg_pca9685_(){
    dev_register_driver((driver_t*)&pca9685_drv);
}
