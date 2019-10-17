//
// Created by atmelfan on 3/31/18.
//

#include <stdio.h>
#include "i2c.h"

#define I2C_DEBUG_PRINT 0

unsigned long i2c_transmit(const i2c_dev_t *i2c, uint8_t addr, uint8_t *tx, uint32_t len) {
    i2c_drv_t* drv = (i2c_drv_t *) i2c->dev.drv;
    if(!drv || !drv->tx)
        return 0;
    return drv->tx(i2c, tx, len);
}

uint32_t i2c_receive(const i2c_dev_t *i2c, uint8_t addr, uint8_t *rx, uint32_t len) {
    i2c_drv_t* drv = (i2c_drv_t *) i2c->dev.drv;
    if(!drv || !drv->tx)
        return 0;
    return drv->rx(i2c, rx, len);
}

uint32_t i2c_write_reg(const i2c_dev_t *i2c, uint8_t addr, uint8_t index, uint8_t *tx, uint32_t len) {
    i2c_drv_t* drv = (i2c_drv_t *) i2c->dev.drv;
    if(!drv || !drv->tx)
        return 0;


#if I2C_DEBUG_PRINT
    for (int i = 0; i < len; ++i) {
        printf("%x ", tx[i]);
    }
    printf("\n");
#endif

    /* Clear RW bit */
    addr &= 0xFE;

    /* Start */
    drv->start(i2c, (uint8_t) (addr | I2c_DEV_WRITE));

    /* Send index */
    drv->tx(i2c, &index, 1);

    /* Write data */
    uint32_t l = drv->tx(i2c, tx, len);

    /* Stop */
    drv->stop(i2c);

    return l;
}

uint32_t i2c_read_reg(const i2c_dev_t *i2c, uint8_t addr, uint8_t index, uint8_t *tx, uint32_t len) {
    i2c_drv_t* drv = (i2c_drv_t *) i2c->dev.drv;
    if(!drv || !drv->tx || !drv->tx)
        return 0;


    /* Clear RW bit */
    addr &= 0xFE;

    /* Start */
    drv->start(i2c, (uint8_t) (uint8_t) (addr | I2c_DEV_WRITE));

    /* Send index */
    drv->tx(i2c, &index, 1);

    /* Start */
    drv->start(i2c, (uint8_t) (uint8_t) (addr | I2c_DEV_READ));

    /* Read data */
    uint32_t l = drv->rx(i2c, tx, len);

    /* Stop */
    drv->stop(i2c);

    return 0;
}
