#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_timer.h"

// Definindo os pinos do sensor HC-SR04
#define TRIG_PIN GPIO_NUM_18
#define ECHO_PIN GPIO_NUM_19

// Definindo tags para logs
static const char *TAG = "trena_digital";

// Variável para armazenar a altura calibrada
float altura_calibrada = 0.0;

// Função para inicializar o sensor
void init_hcsr04() {
    gpio_set_direction(TRIG_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(ECHO_PIN, GPIO_MODE_INPUT);
}

// Função para capturar a distância (em cm)
float get_distance_cm() {
    // Envia um pulso de 10us para o pino TRIG
    gpio_set_level(TRIG_PIN, 1);
    esp_rom_delay_us(10);
    gpio_set_level(TRIG_PIN, 0);

    // Aguarda o pino ECHO subir
    while (gpio_get_level(ECHO_PIN) == 0);
    
    // Marca o tempo de início
    int64_t start_time = esp_timer_get_time();
    
    // Aguarda o pino ECHO descer
    while (gpio_get_level(ECHO_PIN) == 1);
    
    // Marca o tempo final
    int64_t end_time = esp_timer_get_time();

    // Calcula a duração do pulso ECHO
    int64_t pulse_duration = end_time - start_time;

    // A fórmula para converter o tempo do pulso em cm é:
    // distância (em cm) = duração do pulso (em microsegundos) * 0.0343 / 2
    float distance_cm = (pulse_duration * 0.0343) / 2.0;

    return distance_cm;
}

// Tarefa para capturar a altura continuamente
void task_captura_altura(void *pvParameters) {
    while (1) {
        float altura = get_distance_cm();
        ESP_LOGI(TAG, "Altura medida: %.2f cm", altura);
        vTaskDelay(pdMS_TO_TICKS(2000)); // Atualiza a cada 2 segundos
    }
}

// Função para o menu
void mostra_menu() {
    printf("\nMENU:\n");
    printf("[1] - Captura a altura para calibragem\n");
    printf("[2] - Mostra a altura capturada\n");
    printf("[0] - Sair\n");
}

// Tarefa para o menu de opções
void task_menu(void *pvParameters) {
    int opcao;
    while (1) {
        mostra_menu();
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        
        switch (opcao) {
            case 1:
                // Captura a altura para calibragem
                altura_calibrada = get_distance_cm();
                printf("Altura calibrada: %.2f cm\n", altura_calibrada);
                break;
            case 2:
                // Mostra a altura calibrada
                printf("Altura calibrada: %.2f cm\n", altura_calibrada);
                break;
            case 0:
                // Sai do menu
                printf("Saindo...\n");
                vTaskDelete(NULL);
                break;
            default:
                printf("Opção inválida!\n");
                break;
        }

        vTaskDelay(pdMS_TO_TICKS(1000)); // Evita sobrecarga da CPU
    }
}

void app_main() {
    init_hcsr04(); // Inicializa o sensor

    // Cria as tarefas em diferentes núcleos
    xTaskCreatePinnedToCore(task_menu, "task_menu", 2048, NULL, 1, NULL, 0); // Core 0
    xTaskCreatePinnedToCore(task_captura_altura, "task_captura_altura", 2048, NULL, 1, NULL, 1); // Core 1
}
