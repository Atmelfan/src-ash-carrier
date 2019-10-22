//
// Created by atmelfan on 2019-10-22.
//

/**
 * Carrier
 */

#ifndef SRC_CARRIER_GPA_ASH_CARRIER_V1_H
#define SRC_CARRIER_GPA_ASH_CARRIER_V1_H

#include "../board.h"
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/f4/rcc.h>
#include <libopencm3/stm32/f4/nvic.h>

/* Jetson TX2 pins */
#define JETSON_TX2_BATOCP_PORT GPIOA
#define JETSON_TX2_BATOCP_PIN  GPIO8

/* Uart config for SCPI interface */
#define SCPI_USART USART2
#define SCPI_USART_PORT GPIOA
#define SCPI_USART_PINS (GPIO2 | GPIO3)
#define SCPI_USART_AF GPIO_AF7
#define SCPI_USART_RCC RCC_USART2


#endif //SRC_CARRIER_GPA_ASH_CARRIER_V1_H
