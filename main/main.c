#include <stdio.h> //Para a entrada e saída padrão (printf),
#include <freertos/FreeRTOS.h> //Para usar o RTOS
#include <freertos/task.h> //Para criar tarefas
#include <dht.h> //Contém as funções necessárias para trabalhar com o sensor DHT22

//#define SENSOR_TYPE DHT_TYPE_AM2302
//Define SENSOR_TYPE como o tipo correto para o DHT22, que é DHT_TYPE_AM2302

#if defined(CONFIG_EXAMPLE_TYPE_DHT11)
#define SENSOR_TYPE DHT_TYPE_DHT11
#endif
#if defined(CONFIG_EXAMPLE_TYPE_AM2301)
#define SENSOR_TYPE DHT_TYPE_AM2301
#endif
#if defined(CONFIG_EXAMPLE_TYPE_SI7021)
#define SENSOR_TYPE DHT_TYPE_SI7021
#endif

void dht_test(void *pvParameters)//Executada no contexto da tarefa criada em app_main()
{
    float temperature, humidity;

#ifdef CONFIG_EXAMPLE_INTERNAL_PULLUP
    gpio_set_pull_mode(dht_gpio, GPIO_PULLUP_ONLY);
#endif
//A diretiva #ifdef condicionalmente habilita a configuração 
//interna de "pull-up" (resistor de puxar para cima) para o pino do sensor DHT
    while (1)//Loop infinito
    {   //Verifica se a leitura do sensor foi bem sucessida
        if (dht_read_float_data(SENSOR_TYPE, CONFIG_EXAMPLE_DATA_GPIO, &humidity, &temperature) == ESP_OK)
            printf("Humidity: %.1f%% Temp: %.1fC\n", humidity, temperature);
        else
            printf("Could not read data from sensor\n");
        //Vai imprimir a cada 2 segundos
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void app_main()//Primeira função a ser executada no programa
{
    xTaskCreate(dht_test, "dht_test", configMINIMAL_STACK_SIZE * 3, NULL, 5, NULL);
}
/*Cria a tarefa dht_test, que executa a função dht_test definida anteriormente. A tarefa 
é criada com o nome "dht_test", um tamanho de pilha mínimo multiplicado por 3, 
nenhum parâmetro adicional (NULL) e prioridade 5 (0 alta até 31 baixa prioridade)*/