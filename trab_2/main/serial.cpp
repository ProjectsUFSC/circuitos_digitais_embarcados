#include <stdio.h>
#include "serial.h"

#include "driver/uart.h"
#include "driver/gpio.h"

void SERIAL::begin(int taxa) {
    uart_config_t uart_config = {
        .baud_rate = taxa,                      //Bluetooth baud rate 
        .data_bits = UART_DATA_8_BITS,          //8 data bits 
        .parity    = UART_PARITY_DISABLE,       //No parity 
        .stop_bits = UART_STOP_BITS_1,          //1 stop bit 
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,   //No hardware flow 
        .rx_flow_ctrl_thresh = 122,
    };
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_0, &uart_config));
}

SERIAL serial = SERIAL();


