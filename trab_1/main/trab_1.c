#include <stdio.h>
#include "driver/gpio.h"
#include "esp_timer.h"
#include "driver/uart.h"
#include "esp_rom_sys.h" // Para esp_rom_delay_us()
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TRIGGER_GPIO 22  
#define ECHO_GPIO    23   

volatile float basic_height = 0;
volatile float height = 0;
volatile float distance = 0;

// Função para capturar a entrada pela UART
int uart_get_input() {
    uint8_t data[1];
    int len = uart_read_bytes(UART_NUM_0, data, sizeof(data), portMAX_DELAY);
    if (len > 0) {
        printf("%c\n", data[0]);
        return data[0];
    } else {
        printf("!!!!Nenhum dado capturado pela UART!!!\n");
    }
    return -1;
}

// Função que mede a altura/distance com o sensor ultrassônico
void set_height() {
    while (1) {
        gpio_set_level(TRIGGER_GPIO, 0);
        vTaskDelay(pdMS_TO_TICKS(2));

        gpio_set_level(TRIGGER_GPIO, 1);
        esp_rom_delay_us(10); 
        gpio_set_level(TRIGGER_GPIO, 0);

        while (gpio_get_level(ECHO_GPIO) == 0);
        int64_t startTime = esp_timer_get_time();

        while (gpio_get_level(ECHO_GPIO) == 1);
        int64_t endTime = esp_timer_get_time();

        int64_t duration = endTime - startTime;
        distance = duration / 58.0;  // Calcula a distância em cm

        vTaskDelay(pdMS_TO_TICKS(200)); // Delay para n sobrecarregar 
    }
}

float get_distance() {
    return distance;
}

// Função que exibe o menu
void menu() {
    int c;
    while (1) {   
        printf(
            "\nMENU \n"
            "Para realizar a calibragem, instale o sensor na posição final.\n"
            "[1] - Captura a altura para calibragem.\n"
            "[2] - Mostra a altura capturada.\n"
            "[0] - Retorna ao menu principal.\n"
            "Escolha uma opção: ");

        c = uart_get_input();

        switch (c) {
            case '1':
                basic_height = get_distance();
                printf("\nDistância de calibragem: %.2f cm\n", basic_height);
                break;
            case '2':
                height = get_distance();
                float aux = basic_height - height;
                printf("\nA altura é: %.2f cm\n", aux);
                break;
            case '0':
                printf("\nRetornando ao menu principal...\n");
                break;
            default:
                printf("\nOpção inválida. Tente novamente.\n");
                break;
        }
    }
}


void app_main(void) {
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << TRIGGER_GPIO),
        .pull_down_en = 0,
        .pull_up_en = 0
    };
    gpio_config(&io_conf);

    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << ECHO_GPIO);
    gpio_config(&io_conf);
    
    uart_config_t uart_config = {
        .baud_rate = 115200,                    //Bluetooth baud rate
        .data_bits = UART_DATA_8_BITS,          //8 data bits
        .parity    = UART_PARITY_DISABLE,       //No parity
        .stop_bits = UART_STOP_BITS_1,          //1 stop bit 
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE   //No hardware flow
    };

     // se for utilizar a funcao de leitura da uart precisa instalar o driver
    uart_driver_install(UART_NUM_0, 2048, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_0, &uart_config);

    xTaskCreatePinnedToCore(&menu, "menu_task", 2048, NULL, 5, NULL, 0);
    xTaskCreatePinnedToCore(&set_height, "sensor_task", 2048, NULL, 5, NULL, 1);
}