//
// Created by atmelfan on 3/28/18.
//

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <stdio.h>
#include "../dev.h"
#include "../gpio.h"
#include "../log.h"
#include "../gpio.dtsi"

struct {
    uint32_t port;
    enum rcc_periph_clken rcc;
} gpio_ports[] = {
        {GPIOA, RCC_GPIOA},
        {GPIOB, RCC_GPIOB},
        {GPIOC, RCC_GPIOC},
        {GPIOD, RCC_GPIOD},
        {GPIOE, RCC_GPIOE},
        {GPIOF, RCC_GPIOF},
#ifdef GPIOG
        {GPIOG, RCC_GPIOG},
#endif
#ifdef GPIOH
        {GPIOH, RCC_GPIOH},
#endif
#ifdef GPIOI
        {GPIOI, RCC_GPIOI},
#endif
#ifdef GPIOJ
        {GPIOJ, RCC_GPIOJ},
#endif
#ifdef GPIOK
        {GPIOK, RCC_GPIOK},
#endif
        {0, RCC_GPIOA}
};


uint8_t gpio_pull(uint32_t pull){
    switch(pull){
        case DTIO_PULL_UP:
            return GPIO_PUPD_PULLUP;
        case DTIO_PULL_DOWN:
            return GPIO_PUPD_PULLDOWN;
        default:
            return GPIO_PUPD_NONE;
    }
}

#define NUM_GPIO_PINS 16

DEV_PROBE_CALLBACK(stm32f4_gpio, fdt, node, parent){
    gpio_dev_t* gpio = malloc(sizeof(gpio_dev_t));

    /*Generic device info*/
    if(!dev_init_from_fdt(&gpio->dev, fdt, node, DEV_TYPE_GPIO)){
        logd_printfs(LOG_ERROR, "failed to init device_t\n");
        free(gpio);
        return NULL;
    }

    uint32_t reg = gpio->dev.reg;

    for (int i = 0; gpio_ports[i].port ; ++i) {

        /* Look for port */
        if(gpio_ports[i].port == reg){


            fdt_token* ios = fdt_node_get_prop(fdt, node, "io", false);
            /* Initialize if available */
            if(ios){
                uint32_t num_ios = fdt_prop_len(fdt, ios)/sizeof(uint32_t);
                rcc_periph_clock_enable(gpio_ports[i].rcc);

                #define CELLS_PER_IO 5
                if((num_ios % CELLS_PER_IO) != 0){
                    /* Malformed IO init, abort */
                    logd_printfs(LOG_ERROR, "io properties not a multiple of 5!\n");
                    return NULL;
                }


                for (int j = 0; j < num_ios/CELLS_PER_IO && j < NUM_GPIO_PINS; ++j) {
                    uint8_t io =    (uint8_t)(fdt_read_u32(&ios->cells[CELLS_PER_IO * j + 0]) & 0xFFFF);
                    uint8_t mode =  (uint8_t)fdt_read_u32(&ios->cells[CELLS_PER_IO * j + 1]);
                    uint8_t type =  (uint8_t)fdt_read_u32(&ios->cells[CELLS_PER_IO * j + 2]);
                    uint8_t pull =  (uint8_t)fdt_read_u32(&ios->cells[CELLS_PER_IO * j + 3]);
                    uint8_t af =    (uint8_t)(fdt_read_u32(&ios->cells[CELLS_PER_IO * j + 4]) & 0x0F);

                    /* Set output properties */
                    gpio_set_output_options(reg,
                                            (uint8_t) ((type == DTIO_OUTPUT_OD) ? GPIO_OTYPE_OD : GPIO_OTYPE_PP),
                                            GPIO_OSPEED_100MHZ,
                                            (uint16_t) (1 << io));

                    /* Configure IOs*/
                    switch(mode){
                        case DTIO_MODE_AF:
                            gpio_mode_setup(reg, GPIO_MODE_AF, gpio_pull(pull), (uint16_t) (1 << io));
                            gpio_set_af(reg, af, (uint16_t) (1 << io));
                            break;
                        case DTIO_MODE_ANALOG:
                            gpio_mode_setup(reg, GPIO_MODE_ANALOG, gpio_pull(pull), (uint16_t) (1 << io));
                            break;

                        case DTIO_MODE_INPUT:
                            gpio_mode_setup(reg, GPIO_MODE_INPUT, gpio_pull(pull), (uint16_t) (1 << io));
                            break;

                        default: /*DTIO_MODE_OUTPUT*/
                            gpio_mode_setup(reg, GPIO_MODE_OUTPUT, gpio_pull(pull), (uint16_t) (1 << io));
                            if(af == DTIO_LOW)
                                gpio_clear(reg, (uint16_t) (1 << io));
                            else
                                gpio_set(reg, (uint16_t) (1 << io));
                            break;
                    }

                }

            }else{
                logd_printfs(LOG_INFO, "no initialization values.\n");
            }

            dev_register_device((device_t *) gpio);
            logd_printf(LOG_INFO, "initialized %s!\n", gpio->dev.name);
            return (device_t *)gpio;
        }
    }

    logd_printfs(LOG_ERROR, "reg is not a gpio port\n");
    free(gpio);
    return NULL;

}

DEV_COMPAT_LIST(stm32f4_gpio) = {
        "stm32f4,port",
        NULL
};

uint32_t stm32_gpio_getset(gpio_dev_t* dev, uint32_t mask, gpio_dev_action a){
    /* Do appropriate action */
    switch(a){
        case GPIO_DEV_SET:
            gpio_set(dev->dev.reg, (uint16_t) mask);
            break;
        case GPIO_DEV_CLR:
            gpio_clear(dev->dev.reg, (uint16_t) mask);
            break;
        case GPIO_DEV_TGL:
            gpio_toggle(dev->dev.reg, (uint16_t) mask);
            break;
        default:
            break;
    }
    /* Return state */
    return gpio_get(dev->dev.reg, (uint16_t) mask);
}

gpio_dev_mode stm32f4_gpio_mode(gpio_dev_t* dev, uint32_t mask, gpio_dev_mode mod){
    switch(mod){
        case GPIO_DEV_INPUT:
            gpio_mode_setup(dev->dev.reg, GPIO_MODE_INPUT, GPIO_PUPD_NONE, (uint16_t) mask);
            break;
        case GPIO_DEV_OUTPUT:
            gpio_mode_setup(dev->dev.reg, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, (uint16_t) mask);
            break;
        case GPIO_DEV_ANALOG:
            gpio_mode_setup(dev->dev.reg, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, (uint16_t) mask);
            break;
        case GPIO_DEV_ALTF:
            gpio_mode_setup(dev->dev.reg, GPIO_MODE_AF, GPIO_PUPD_NONE, (uint16_t) mask);
            break;
        default:
            return GPIO_DEV_NOMODE;
    }
    return mod;
}



static gpio_driver_t stm32f4_gpio_drv = {
        /* Implement super */
        DRIVER_SUPER(driver_t, drv) = {
                .compatible = stm32f4_gpio_compat,
                .probe = stm32f4_gpio_probe,
                .close = NULL
        },
        .getset = stm32_gpio_getset,
        .mode = stm32f4_gpio_mode,
        .type = NULL,//TODO implement .type & .af
        .af = NULL

};

DEV_REGISTER_CALLBACK(stm32f4_gpio){
    dev_register_driver((driver_t *) &stm32f4_gpio_drv);
}


