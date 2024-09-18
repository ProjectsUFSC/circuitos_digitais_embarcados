#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

// Programa faz um semaforo com 3 LEDs


// Defina os pinos dos LEDs
#define LED_PIN1 GPIO_NUM_2  // LED 1 no GPIO 2
#define LED_PIN2 GPIO_NUM_4  // LED 2 no GPIO 4
#define LED_PIN3 GPIO_NUM_16 // LED 3 no GPIO 16

#define LOW 0
#define HIGH 1


void blink_leds(void *pvParameter) {
    while (1) {
        gpio_set_level(LED_PIN1, HIGH);
        vTaskDelay(pdMS_TO_TICKS(5000));
        gpio_set_level(LED_PIN1, LOW);
        gpio_set_level(LED_PIN2, HIGH);
        vTaskDelay(pdMS_TO_TICKS(1000));
        gpio_set_level(LED_PIN2, LOW);
        gpio_set_level(LED_PIN3, HIGH);
        vTaskDelay(pdMS_TO_TICKS(5000));
        gpio_set_level(LED_PIN3, LOW);
        
    }
}



void app_main() {
    // Configura os pinos dos LEDs como saída
    gpio_reset_pin(LED_PIN1);
    gpio_set_direction(LED_PIN1, GPIO_MODE_OUTPUT);

    gpio_reset_pin(LED_PIN2);
    gpio_set_direction(LED_PIN2, GPIO_MODE_OUTPUT);

    gpio_reset_pin(LED_PIN3);
    gpio_set_direction(LED_PIN3, GPIO_MODE_OUTPUT);

    // Cria tarefas para piscar os LEDs em frequências diferentes
    xTaskCreate(&blink_leds, "blink_leds", 1024, NULL, 5, NULL);
}
