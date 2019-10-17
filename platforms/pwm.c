//
// Created by atmelfan on 4/1/18.
//

#include "pwm.h"

pwm_status set_pwm(pwm_dev_t *dev, uint16_t ch, uint32_t off, uint32_t on) {


    /* Check if valid PWM device*/
    if(!dev || dev->dev.type != DEV_TYPE_PWM)
        return PWM_UNKNOWN;

    /* Call driver function */
    pwm_driver_t* drv = (pwm_driver_t*)dev->dev.drv;

    if(!drv)
        return PWM_UNKNOWN;

    return drv->set_pwm(dev, ch, off, on);
}

pwm_status set_duty(pwm_dev_t *dev, uint16_t ch, float duty) {
    /* Check if valid PWM device*/
    if(!dev || dev->dev.type != DEV_TYPE_PWM)
        return PWM_UNKNOWN;

    if(duty > 1.0f)
        return PWM_OUTATIME;

    /* Call driver function */
    pwm_driver_t* drv = (pwm_driver_t*)dev->dev.drv;
    return drv->set_pwm(dev, ch, 0, (uint32_t) (duty * drv->period));
}
