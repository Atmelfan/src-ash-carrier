//
// Created by atmelfan on 2019-10-22.
//


#include "gpa_ash_carrier_v1.h"


/**
 * Setup gpio for the Jetson TX2
 */
void init_jetson(){

    /* Battery OCP pin, configure as OD and with pull-up*/
    gpio_mode_setup(JETSON_TX2_BATOCP_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, JETSON_TX2_BATOCP_PIN);
    gpio_set_output_options(JETSON_TX2_BATOCP_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_100MHZ, JETSON_TX2_BATOCP_PIN);
    gpio_set(JETSON_TX2_BATOCP_PORT, JETSON_TX2_BATOCP_PIN);
}

void init_scpi_usart(){
    gpio_mode_setup(SCPI_USART_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, SCPI_USART_PINS);
    gpio_set_af(SCPI_USART_PORT, SCPI_USART_AF, SCPI_USART_PINS);


    /* This then enables the clock to the USART1 peripheral which is
     * attached inside the chip to the APB1 bus. Different peripherals
     * attach to different buses, and even some UARTS are attached to
     * APB1 and some to APB2, again the data sheet is useful here.
     */
    rcc_periph_clock_enable(SCPI_USART_RCC);

    /* Set up USART/UART parameters using the libopencm3 helper functions */
    usart_set_baudrate(SCPI_USART, 115200);
    usart_set_databits(SCPI_USART, 8);
    usart_set_stopbits(SCPI_USART, USART_STOPBITS_1);
    usart_set_mode(SCPI_USART, USART_MODE_TX_RX);
    usart_set_parity(SCPI_USART, USART_PARITY_NONE);
    usart_set_flow_control(SCPI_USART, USART_FLOWCONTROL_NONE);
    usart_enable(SCPI_USART);

    /* Enable interrupts from the USART */
    nvic_enable_irq(NVIC_USART2_IRQ);

    /* Specifically enable recieve interrupts */
    usart_enable_rx_interrupt(SCPI_USART);
}


/* Set STM32 to 168 MHz. */
static void clock_setup(void)
{
    rcc_clock_setup_pll(&rcc_hse_25mhz_3v3[RCC_CLOCK_3V3_168MHZ]);
}

void board_init(){

    /* Use semihosting if in debug */
#ifdef DEBUG
    extern void initialise_monitor_handles(void);
    initialise_monitor_handles();
#endif

    clock_setup();

    init_jetson();

    init_scpi_usart();


}

void board_update(){

}

void sys_tick_handler(){

}

