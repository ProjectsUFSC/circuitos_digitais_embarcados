
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"


// Codigo executa tarefas em diferentes cores do processador do ESP32


void meu_delayA (void)
{
    size_t i = 0;
    while(i < 10000000)
    {
        i++;
    }
	printf("delay task1\n");

}


void meu_delayV (void)
{
    size_t i = 0;
    while(i < 10000000)
    {
        i++;
    }
	printf("delay task2\n");
}


void task1(void *pvParameter)
{
	while(1)
	{
		printf("Inicio da task1\n");
		meu_delayA();
		meu_delayA();
		printf("Fim da task1\n***********");
		vTaskDelay(pdMS_TO_TICKS(10000));

	}
}
void task2(void *pvParameter)
{
	while(1)
	{
		printf("Inicio da task2\n");
	   	meu_delayV();
		meu_delayV();
		printf("Fim da task2\n----------------");
		vTaskDelay(pdMS_TO_TICKS(10000));
	}
}
void app_main()
{

    xTaskCreatePinnedToCore(task1, "tarefa_1", 2048, NULL, 5, NULL, 1);
    xTaskCreatePinnedToCore(task2, "tarefa_2", 2048, NULL, 5, NULL, 0);

}
    