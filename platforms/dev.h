//
// Created by atmelfan on 2018-03-21.
//

#ifndef SRC_CARRIER_DEV_H
#define SRC_CARRIER_DEV_H


#include <stdint.h>
#include "../fdt/dtb_parser.h"

typedef enum {
    DEV_TYPE_NULL = 0,  /*Unknown or invalid device*/
    DEV_TYPE_SERIAL,    /*Serial device*/
    DEV_TYPE_I2C,       /*I2C controller device*/
    DEV_TYPE_I2C_DEV,   /*I2C device*/
    DEV_TYPE_SPI,       /*Serial Peripheral Interface device*/
    DEV_TYPE_GPIO,      /*General Purpose IO device*/
    DEV_TYPE_PWM,       /*Pulse Width Modulation device*/
    DEV_TYPE_TIM        /*Timer device*/
} dev_type;

/* Return codes used with dev.probe() */
typedef enum {
    PROBE_SUCCESS = 0,
    PROBE_FAIL
} dev_probe_t;

typedef struct device_t device_t;

typedef struct driver_t {
    char** compatible;
    /**
     * Probes a device and if necessary any sub devices
     * @param fdt
     * @param node
     * @param dev
     * @return
     */
    device_t* (*probe)(fdt_header_t *fdt, fdt_token *node, device_t* parent);

    /**
     *
     * @param dev
     * @return
     */
    dev_probe_t (*close)(device_t *dev);

    /**
     * Used in a linked list of drivers. Will be set when registered with dev_register_driver()
     */
    struct driver_t* next;
} driver_t;

#define DRIVER_EXTEND(name, as) name as
#define DRIVER_SUPER(name, as) .as

struct device_t {
    /* Name of device */
    char* name;

    /* Type of device if standard */
    dev_type type;

    /* phandle of device, used to refer to it */
    uint32_t phandle;

    /* Address of device, device dependant */
    uint32_t reg;

    /* Driver associated with device */
    driver_t* drv;

    /* Next device in a linked list */
    device_t* next;

};
#define DEVICE_EXTEND(name, as) name as
#define DEVICE_SUPER(name, as) .as
#define DEVICE_CAST(dev, as) ((as)(dev))

#define PHANDLE_NULL 0

/**
 *
 * @param d
 */
void dev_register_driver(driver_t* d);

/**
 *
 * @param d
 */
void dev_register_device(device_t* d);

/**
 * Adds all static drivers to the linked list
 */
void dev_init_drivers(void);

/**
 *
 * @param compatible
 * @return
 */
driver_t* dev_find_driver(char* compatible);

/**
 * Find a device by its phandle
 * @param phandle, phandle of device, must not be equal to 0
 * @return pointer to device if found, NULL otherwise.
 */
device_t* dev_find_device_phandle(uint32_t phandle);

/**
 * Find a device by its reg
 * @param reg, reg of device
 * @return pointer to device if found, NULL otherwise.
 */
device_t* dev_find_device_reg(uint32_t reg);

/**
 * Probe all devices (nodes with a 'compatible' property) inside a node
 * @param fdt
 * @param node
 * @param parent
 * @return
 */
uint32_t dev_probe_devices_in_fdt(fdt_header_t *fdt, fdt_token *node, device_t *parent);

/**
 *
 * @param d
 * @param fdt
 * @param node
 * @param typ
 */
bool dev_init_from_fdt(device_t *d, fdt_header_t *fdt, fdt_token *node, dev_type typ);


#define SYSTICKS_PER_SECOND 1000

void dev_systick(void);

uint32_t dev_systick_get(void);

void dev_systick_wait(uint32_t c);


#define DEV_T_INIT() (dev_t){.name = NULL, type = DEV_TYPE_NULL, .phandle = 0}

#define DEV_PROBE_CALLBACK(name, fdt, node, parent) device_t* name##_probe(fdt_header_t* fdt, fdt_token* node, device_t* parent)
#define DEV_REGISTER_CALLBACK(name) void reg_##name##_(void)
#define DEV_COMPAT_LIST(name) static char* name##_compat[]
#define DEV_COMPAT_LIST_END NULL

#define DEV_DRIVER_STRUCT(name) static struct driver_t name##_drv

#endif //SRC_CARRIER_DEV_H
