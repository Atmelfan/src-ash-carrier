//
// Created by atmelfan on 2018-03-10.
//

#ifndef SRC_CARRIER_JETSON_H
#define SRC_CARRIER_JETSON_H


#include <stdbool.h>

/**
 * Toggles power button
 */
void jetson_toggle_pwr(void);

/**
 * Toggles the power button for 10s to force shutdown
 */
void jetson_force_off(void);

/**
 * Sets battery charge signal
 * @param set
 */
void jetson_batchg(bool set);

/**
 * Sets battery low signal
 * @param set
 */
void jetson_batlow(bool set);

/**
 * Sets battery overcurrent signal
 * @param set
 */
void jetson_batocp(bool set);

#endif //SRC_CARRIER_JETSON_H
