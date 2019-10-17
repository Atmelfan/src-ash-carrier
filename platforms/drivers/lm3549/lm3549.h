//
// Created by atmelfan on 2018-03-24.
//

#ifndef SRC_CARRIER_LM3549_H
#define SRC_CARRIER_LM3549_H


#include "../../dev.h"
#include "../../pwm.h"
#include "../../i2c.h"

/*
 * Do not need to create a special driver struct since
 * pwm_driver_t contains all necessary info/functions
 *
 */
typedef struct {
    DEVICE_EXTEND(pwm_dev_t, dev);
    i2c_dev_t* i2c;
} lm3549_dev_t;

void reg_lm3549_(void);


#endif //SRC_CARRIER_LM3549_H
