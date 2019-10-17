//
// Created by atmelfan on 4/15/18.
//

#ifndef SRC_CARRIER_SYSTICK_H
#define SRC_CARRIER_SYSTICK_H

#include "dev.h"

/**
 *
 */
typedef struct {
    DEVICE_EXTEND(device_t, dev);
} systick_dev_t;

typedef struct {
    DRIVER_EXTEND(driver_t, drv);

} systick_drv_t;

#endif //SRC_CARRIER_SYSTICK_H
