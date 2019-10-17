//
// Created by atmelfan on 2018-03-21.
//

#ifndef SRC_CARRIER_SERIAL_H
#define SRC_CARRIER_SERIAL_H

#include <stdint-gcc.h>
#include "dev.h"

typedef struct {
    DEVICE_EXTEND(device_t, dev);

} ser_dev_t;

typedef struct {
    DRIVER_EXTEND(driver_t, drv);
    uint32_t (*tx)(const ser_dev_t* dev, uint8_t* tx, uint32_t len);
    uint32_t (*rx)(const ser_dev_t* dev, uint8_t* rx, uint32_t len);
    uint32_t baudrate;
    uint32_t parity;
    uint32_t stop;
} ser_drv_t;

#endif //SRC_CARRIER_SERIAL_H
