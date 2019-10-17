//
// Created by atmelfan on 2018-03-21.
//

#include <libopencm3/stm32/common/rcc_common_all.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include "../serial.h"
#include "../log.h"


static ser_drv_t stm32f4_ser_drv;

bool stm32f4_ser_init(ser_dev_t *ser, uint32_t speed, uint32_t parity, uint32_t stop) {
    if(ser->dev.type != DEV_TYPE_I2C)
        return false;

    /*Check and enable clock*/
    enum rcc_periph_clken clken;
    switch(ser->dev.reg){
        case USART1:
            clken = RCC_USART1;
            break;
#ifdef USART2
        case USART2:
            clken = RCC_USART2;
            break;
#endif
#ifdef USART3
        case USART3:
            clken = RCC_USART3;
            break;
#endif
#ifdef UART4
        case UART4:
            clken = RCC_UART4;
            break;
#endif
#ifdef UART5
        case UART5:
            clken = RCC_UART5;
            break;
#endif
#ifdef USART6
        case USART6:
            clken = RCC_USART6;
            break;
#endif
#ifdef UART7
        case UART7:
            clken = RCC_UART7;
            break;
#endif
#ifdef UART8
        case UART8:
            clken = RCC_UART8;
            break;
#endif
        default:
            return false;
    }
    rcc_periph_clock_enable(clken);
    usart_set_baudrate(ser->dev.reg, speed);
    usart_set_databits(ser->dev.reg, 8);
    switch (stop){
        case 1:
            usart_set_stopbits(ser->dev.reg, USART_STOPBITS_1);
            break;

        case 2:
            usart_set_stopbits(ser->dev.reg, USART_STOPBITS_2);
            break;

        default:
            usart_set_stopbits(ser->dev.reg, USART_STOPBITS_1);
            break;

    }
    usart_set_mode(ser->dev.reg, USART_MODE_TX_RX);
    usart_set_parity(ser->dev.reg, USART_PARITY_NONE);
    usart_set_flow_control(ser->dev.reg, USART_FLOWCONTROL_NONE);
    return true;
}

uint32_t stm32f4_ser_tx(const ser_dev_t* dev, uint8_t* tx, uint32_t len){
    /**/
    for (int i = 0; i < len; ++i) {
        usart_send_blocking(dev->dev.reg, (uint16_t)tx);
        tx++;
    }

    return len;
}

uint32_t stm32f4_ser_rx(const ser_dev_t* dev, uint8_t* rx, uint32_t len){

    /**/
    for (int i = 0; i < len; ++i) {
        *rx = (uint8_t)usart_recv_blocking(dev->dev.reg);
        rx++;
    }

    return len;
}

DEV_PROBE_CALLBACK(stm32f4_ser, fdt, node, parent){

    ser_dev_t* ser = malloc(sizeof(ser_dev_t));

    /*Generic device info*/
    if(!dev_init_from_fdt(&ser->dev, fdt, node, DEV_TYPE_SERIAL)){
        logd_printfs(LOG_ERROR, "failed to init device_t\n");
        free(ser);
        return NULL;
    }

    /* Add driver info */
    ser->dev.drv = (driver_t*)&stm32f4_ser_drv;

    /* Get speed (default = 100KHz) */
    uint32_t baudrate = fdt_node_get_u32(fdt, node, "baudrate", 9600);
    if(baudrate > 348000){
        logd_printfs(LOG_WARNING, "baudrate out of range\n");
    }

    /*TODO: Fix GPIO API & PINS!*/

    /* Init peripheral */
    if(!stm32f4_ser_init(ser, baudrate, 0, 0)){
        logd_printfs(LOG_ERROR, "failed to init serial peripheral\n");
        free(ser);
        return NULL;
    }


    /* Register device if successfull */
    dev_register_device(&ser->dev);
    log_printf(&default_log, LOG_INFO, "initialized %s!\n", ser->dev.name);

    return (device_t *) ser;
}

/* List of compatibility strings */
DEV_COMPAT_LIST(stm32f4_ser) = {
        "stm32f4,ser",
        DEV_COMPAT_LIST_END
};

/**
 * Driver struct extends the base driver_t
 */
static ser_drv_t stm32f4_ser_drv = {
        /* Implement super */
        DRIVER_SUPER(driver_t, drv) = {
                .compatible = stm32f4_ser_compat,
                .probe = stm32f4_ser_probe,
                .close = NULL
        },
        /* Transmit/Receive functions */
        .rx = stm32f4_ser_rx,
        .tx = stm32f4_ser_tx,
};



DEV_REGISTER_CALLBACK(stm32f4_i2c){
    dev_register_driver((driver_t*)&stm32f4_ser_drv);
}



