//
// Created by atmelfan on 2018-03-10.
//

#include <libopencm3/stm32/gpio.h>
#include "jetson.h"

void jetson_toggle_pwr(void){

}

void jetson_force_off(void){

}

void jetson_batchg(bool set){
    (void)set;
}

void jetson_batlow(bool set){
    (void)set;
}

void jetson_batocp(bool set){
    if(set)
        gpio_set(GPIOA, GPIO8);
    else
        gpio_clear(GPIOA, GPIO8);
}
