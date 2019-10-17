//
// Created by atmelfan on 2018-03-21.
//

#include "dev.h"
#include "log.h"
#include "../fdt/dtb_parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static struct driver_t* drivers = NULL;
static struct device_t* devices = NULL;

#define REG_DRIVER(name) extern void reg_##name##_(struct driver_t*); reg_##name##_(drivers)

void dev_register_driver(driver_t* d){
    if(d == NULL)
        return;


    if(drivers == NULL){
        /*Init list with first member*/
        drivers = d;
    }else{
        /*Add member to list*/
        driver_t* t = drivers;
        for ( ; t->next; t = t->next) { }
        t->next = d;
    }
    d->next = NULL;
}

void dev_register_device( device_t* d){
    if(d == NULL)
        return;

    if(devices == NULL){
        devices = d;
    }else{
        device_t* t = devices;
        for (; t->next; t = t->next) { }
        t->next = d;
    }
    d->next = NULL;
}

void dev_init_drivers(){
    REG_DRIVER(stm32f4_gpio);
    REG_DRIVER(stm32f4_i2c);
    REG_DRIVER(stm32f4_systick);
    REG_DRIVER(pca9685);
}

driver_t* dev_find_driver(char* compatible){
    if(!compatible)
        return NULL;

    for(struct driver_t* c = drivers; c ; c = c->next){
        for(char** compat = c->compatible; *compat; compat++){
            if(!strcmp(*compat, compatible))
                return c;
        }
    }
    return NULL;
}

bool dev_init_from_fdt(device_t *d, fdt_header_t *fdt, fdt_token *node, dev_type typ){
    if(d == NULL || fdt == NULL){
        return false;
    }

    /* Get device nme from node name */
    d->name = node->name;
    /*Get address of device*/
    d->reg = fdt_node_get_u32(fdt, node, "reg", 0);
    /*Get phandle other devices can find or set it to NULL*/
    d->phandle = fdt_node_get_u32(fdt, node, "phandle", PHANDLE_NULL);
    /*Set appropriate type so that other devices know how to deal with it*/
    d->type = typ;

    d->drv = NULL;

    d->next = NULL;

    return true;
}

uint32_t dev_probe_devices_in_fdt(fdt_header_t *fdt, fdt_token *node, device_t *parent) {

    uint32_t num_devices = 0;
    for(fdt_token* d = fdt_token_next(fdt, node); fdt_token_get_type(d) != FDT_END_NODE ; d = fdt_token_next(fdt, d)){
        /*Check for nodes*/
        if(fdt_token_get_type(d) == FDT_BEGIN_NODE){
            char* compatible = fdt_node_get_str(fdt, d, "compatible", NULL);

            /* No compatible property, skip*/
            if(!compatible){
                d = fdt_node_end(fdt, d);
                continue;
            }

            logd_push(d->name);
            /* Find driver */
            driver_t* drv = dev_find_driver(compatible);
            if(drv){
                /*Try to probe device*/
                device_t* x = drv->probe(fdt, d, parent);
                if(x){
                    dev_probe_devices_in_fdt(fdt, d, x);

                }else{
                    logd_printfs(LOG_ERROR, "*Failed to probe device!\n");
                }
            }else{
                logd_printfs(LOG_ERROR, "*No driver for device!\n");
            }

            logd_pop();

            num_devices++;

            /*Exit node*/
            d = fdt_node_end(fdt, d);
        }
    }
    return num_devices;
}

device_t *dev_find_device_phandle(uint32_t phandle) {
    for (device_t* t = devices; t; t = t->next) {
        if(t->phandle == phandle)
            return t;
    }
    return NULL;
}

device_t *dev_find_device_reg(uint32_t reg) {
    for (device_t* t = devices; t; t = t->next) {
        if(t->reg == reg)
            return t;
    }
    return NULL;
}

volatile uint32_t systick_counts = 0;

void dev_systick(void) {
    systick_counts++;
}

uint32_t dev_systick_get(void) {
    return systick_counts;
}

void dev_systick_wait(uint32_t c) {
    uint32_t l = systick_counts;
    while(systick_counts - l < c){ }
}
