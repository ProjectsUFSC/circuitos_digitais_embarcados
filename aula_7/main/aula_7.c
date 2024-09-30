#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "esp_timer.h"


#define PINO 15
    
volatile int valor=0;
volatile int64_t tempo1=0;
volatile int64_t tempo2=0;


void  INTERRUPCAO (void *)
{
   if(valor == 0){
       tempo1 = esp_timer_get_time();
       valor = 1;
   }else{ 
       tempo2 = esp_timer_get_time();
        valor = 0;
   }
    

}

void print_dist(void *)
{
    int64_t valor_aux = 0;
    while(true){
        valor_aux = tempo2 - tempo1;
        printf("Tempo: %lld\n", valor_aux);
        printf("tempo1: %lld\n", tempo1);
        printf("tempo2: %lld\n", tempo2);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    

}

void app_main(void)
{

    /* Configura a parte de pinos de IO */
    gpio_set_direction(PINO, GPIO_MODE_INPUT);
    gpio_set_pull_mode(PINO, GPIO_PULLUP_ONLY);


    // Configura a parte de iterrupcoes
    gpio_install_isr_service(1);
    gpio_set_intr_type(PINO, GPIO_INTR_ANYEDGE);
    gpio_isr_handler_add(PINO, INTERRUPCAO, NULL);

    xTaskCreate(&print_dist, "distancia", 2048, NULL, 1, NULL);

}
