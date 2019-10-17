//
// Created by atmelfan on 3/28/18.
//

#ifndef SRC_CARRIER_GPIO_H
#define SRC_CARRIER_GPIO_H

#include "dev.h"


typedef enum {
    GPIO_DEV_INPUT = 0,
    GPIO_DEV_OUTPUT,
    GPIO_DEV_ANALOG,
    GPIO_DEV_ALTF,
    GPIO_DEV_NOMODE,
} gpio_dev_mode;

typedef enum {
    GPIO_DEV_PUSHPULL = 0,
    GPIO_DEV_OPENDRAIN,
    GPIO_DEV_NOTYPE
} gpio_dev_type;

typedef enum {
    GPIO_DEV_GET = 0,
    GPIO_DEV_SET,
    GPIO_DEV_CLR,
    GPIO_DEV_TGL
} gpio_dev_action;

typedef struct {
    device_t dev;
} gpio_dev_t;

typedef struct {
    DRIVER_EXTEND(driver_t, drv);
    uint32_t (*getset)(gpio_dev_t* dev, uint32_t mask, gpio_dev_action a);
    gpio_dev_mode (*mode)(gpio_dev_t* dev, uint32_t mask, gpio_dev_mode mod);
    gpio_dev_type (*type)(gpio_dev_t* dev, uint32_t mask, gpio_dev_type typ);
    uint8_t (*af)(gpio_dev_t* dev, uint32_t mask, uint8_t af);

} gpio_driver_t;

/**
 * Read from port device with mask
 * @param gpio_dev
 * @param mask
 * @return
 */
uint32_t gpio_dev_read(gpio_dev_t* gpio_dev, uint32_t mask);

/**
 * Set, clear or toggle port device depending on mask
 * @param gpio_dev
 * @param mask
 * @return
 */
uint32_t gpio_dev_set(gpio_dev_t *gpio_dev, uint32_t mask);
uint32_t gpio_dev_clr(gpio_dev_t *gpio_dev, uint32_t mask);
uint32_t gpio_dev_tgl(gpio_dev_t *gpio_dev, uint32_t mask);

/**
 *
 * @param gpio_dev
 * @param mask
 * @param mode
 * @return
 */
bool gpio_dev_setmode(gpio_dev_t* gpio_dev, uint32_t mask, gpio_dev_mode mode);

/**
 *
 * @param gpio_dev
 * @param mask
 * @param af
 * @return
 */
bool gpio_dev_setaf(gpio_dev_t* gpio_dev, uint32_t mask, uint8_t af);

#endif //SRC_CARRIER_GPIO_H
