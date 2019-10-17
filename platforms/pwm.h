//
// Created by atmelfan on 4/1/18.
//

#ifndef SRC_CARRIER_PWM_H
#define SRC_CARRIER_PWM_H

#include "dev.h"

/**
 * PWM device, implements a generic pwm interface
 */
typedef struct {
    DEVICE_EXTEND(device_t, dev);
} pwm_dev_t;


typedef enum {
    /*No error */
    PWM_OK = 0,
    /* (on + off) > period */
    PWM_OUTATIME,
    /* ch >= num_channels */
    PWM_OUTACHANNELS,
    /* Off period not implemented and not 0*/
    PWM_OFFNOI,
    /* Other errors */
    PWM_UNKNOWN
} pwm_status;

typedef struct {
    DRIVER_EXTEND(driver_t, drv);

    /* Number of channels supported */
    uint16_t num_channels;

    /* Maximum period */
    uint32_t period;

    /**
     * Set pwm duty on channel
     * @param ch, channel to set
     * @param off, initial off period (typically 0), may be ignored
     * @param on, on period. The result of (on + off) must be less or equal to period
     */
    pwm_status (*set_pwm)(pwm_dev_t* dev, uint16_t ch, uint32_t off, uint32_t on);
} pwm_driver_t;

pwm_status set_pwm(pwm_dev_t* dev, uint16_t ch, uint32_t off, uint32_t on);

pwm_status set_duty(pwm_dev_t* dev, uint16_t ch, float duty);


#endif //SRC_CARRIER_PWM_H
