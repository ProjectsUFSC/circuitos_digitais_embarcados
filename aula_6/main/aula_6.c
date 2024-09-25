#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "driver/gpio.h"

#define PINO 23
    
volatile int valor=0;

void  INTERRUPCAO (void *)
{
    valor++;
}

void print_freq(void *)
{
    int valor_print = 0;
    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
        valor_print = valor;
        valor = 0;
        printf("O numero de pulsos em 1s foi -> %d\n", valor_print);

    }
    

}

void app_main(void)
{

    /* Configura a parte de pinos de IO */
    gpio_set_direction(PINO, GPIO_MODE_INPUT);
    gpio_set_pull_mode(PINO, GPIO_PULLUP_ONLY);


    // Configura a parte de iterrupcoes
    gpio_install_isr_service(1);
    gpio_set_intr_type(PINO, GPIO_INTR_NEGEDGE);
    gpio_isr_handler_add(PINO, INTERRUPCAO, NULL);

    xTaskCreate(&print_freq, "frequencia", 2048, NULL, 1, NULL);

}
