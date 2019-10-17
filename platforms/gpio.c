//
// Created by atmelfan on 4/5/18.
//

#include "gpio.h"
#include <libopencm3/stm32/gpio.h>


uint32_t gpio_dev_read(gpio_dev_t *gpio_dev, uint32_t mask) {
    gpio_driver_t* drv = (gpio_driver_t *) gpio_dev->dev.drv;
    if(!drv || !drv->getset)
        return 0;
    return drv->getset(gpio_dev, mask, GPIO_DEV_GET);
}

uint32_t gpio_dev_set(gpio_dev_t *gpio_dev, uint32_t mask) {
    gpio_driver_t* drv = (gpio_driver_t *) gpio_dev->dev.drv;
    if(!drv || !drv->getset)
        return 0;
    return drv->getset(gpio_dev, mask, GPIO_DEV_SET);
}

uint32_t gpio_dev_clr(gpio_dev_t *gpio_dev, uint32_t mask) {
    gpio_driver_t* drv = (gpio_driver_t *) gpio_dev->dev.drv;
    if(!drv || !drv->getset)
        return 0;
    return drv->getset(gpio_dev, mask, GPIO_DEV_CLR);
}

uint32_t gpio_dev_tgl(gpio_dev_t *gpio_dev, uint32_t mask) {
    gpio_driver_t* drv = (gpio_driver_t *) gpio_dev->dev.drv;
    if(!drv || !drv->getset)
        return 0;
    return drv->getset(gpio_dev, mask, GPIO_DEV_TGL);
}

bool gpio_dev_setmode(gpio_dev_t *gpio_dev, uint32_t mask, gpio_dev_mode mode) {
    gpio_driver_t* drv = (gpio_driver_t *) gpio_dev->dev.drv;
    if(!drv || !drv->getset)
        return false;
    return drv->mode(gpio_dev, mask, mode) != GPIO_DEV_NOMODE;
}

bool gpio_dev_setaf(gpio_dev_t *gpio_dev, uint32_t mask, uint8_t af) {
    gpio_driver_t* drv = (gpio_driver_t *) gpio_dev->dev.drv;
    if(!drv || !drv->getset)
        return false;
    return drv->af(gpio_dev, mask, af) != af;
}
