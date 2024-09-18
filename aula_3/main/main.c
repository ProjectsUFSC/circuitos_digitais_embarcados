#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

// Faz dois leds piscatem em frequencias diferentes


// Defina os pinos dos LEDs
#define LED_PIN1 GPIO_NUM_2  // LED 1 no GPIO 2
#define LED_PIN2 GPIO_NUM_4  // LED 2 no GPIO 4

// Defina os tempos de intervalo (em milissegundos)
#define INTERVAL1 500  // Frequência de 1 Hz (500ms ligado, 500ms desligado)
#define INTERVAL2 1000 // Frequência de 0,5 Hz (1000ms ligado, 1000ms desligado)

void blink_led1(void *pvParameter) {
    while (1) {
        // Alterna o estado do LED 1
        gpio_set_level(LED_PIN1, 1);
        vTaskDelay(INTERVAL1 / portTICK_PERIOD_MS);
        gpio_set_level(LED_PIN1, 0);
        vTaskDelay(INTERVAL1 / portTICK_PERIOD_MS); // desatualizado usar agora vTaskDelay(pdMS_TO_TICKS(INTERVAL1)); 1s em ticks é 1000
    }
}

void blink_led2(void *pvParameter) {
    while (1) {
        // Alterna o estado do LED 2
        gpio_set_level(LED_PIN2, 1);
        vTaskDelay(INTERVAL2 / portTICK_PERIOD_MS);
        gpio_set_level(LED_PIN2, 0);
        vTaskDelay(INTERVAL2 / portTICK_PERIOD_MS);
    }
}

void app_main() {
    // Configura os pinos dos LEDs como saída
    gpio_reset_pin(LED_PIN1);
    gpio_set_direction(LED_PIN1, GPIO_MODE_OUTPUT);

    gpio_reset_pin(LED_PIN2);
    gpio_set_direction(LED_PIN2, GPIO_MODE_OUTPUT);

    // Cria tarefas para piscar os LEDs em frequências diferentes
    xTaskCreate(&blink_led1, "blink_led1", 1024, NULL, 5, NULL);
    xTaskCreate(&blink_led2, "blink_led2", 1024, NULL, 5, NULL);
}
