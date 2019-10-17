//
// Created by atmelfan on 2018-03-21.
//

#ifndef SRC_CARRIER_I2C_H
#define SRC_CARRIER_I2C_H

#include <stdint.h>
#include <stdbool.h>
#include "dev.h"
#include "i2c.dtsi"

#define I2c_DEV_READ  0x01
#define I2c_DEV_WRITE 0x00

/**
 *
 */
typedef struct {
    DEVICE_EXTEND(device_t, dev);
    uint8_t own_address;
    uint32_t speed;
} i2c_dev_t;

typedef struct {
    DRIVER_EXTEND(driver_t, drv);
    uint32_t (*start)(const i2c_dev_t* dev, uint8_t addr);
    uint32_t (*tx)(const i2c_dev_t* dev, uint8_t* tx, uint32_t len);
    uint32_t (*rx)(const i2c_dev_t* dev, uint8_t* rx, uint32_t len);
    uint32_t (*stop)(const i2c_dev_t* dev);
} i2c_drv_t;

/**
 * Transmit bytes over a i2c device
 * @param i2c, device to be used
 * @param addr, address of i2c device
 * @param tx, pointer to transmit buffer
 * @param len, num bytes to transmit
 * @return num bytes transmitted
 */
uint32_t i2c_transmit(const i2c_dev_t* i2c, uint8_t addr, uint8_t* tx, uint32_t len);

/**
 * Receive bytes over a i2c device
 * @param i2c, i2c device to be used
 * @param addr, address of i2c device
 * @param rx, pointer to receive buffer
 * @param len, num bytes to receive
 * @return num bytes received
 */
uint32_t i2c_receive(const i2c_dev_t* i2c, uint8_t addr, uint8_t* rx, uint32_t len);

/**
 * Write to one or more registers over a i2c device
 * @param i2c
 * @param addr
 * @param index
 * @param tx
 * @param len
 * @return num bytes written
 */
uint32_t i2c_write_reg(const i2c_dev_t* i2c, uint8_t addr, uint8_t index, uint8_t* tx, uint32_t len);

/**
 * Read from one or more registers over a i2c device
 * @param i2c
 * @param addr
 * @param index
 * @param tx
 * @param len
 * @return num bytes read
 */
uint32_t i2c_read_reg(const i2c_dev_t* i2c, uint8_t addr, uint8_t index, uint8_t* tx, uint32_t len);

#endif //SRC_CARRIER_I2C_H

