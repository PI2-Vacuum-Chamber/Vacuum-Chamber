#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/timers.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "lwip/igmp.h"

#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "soc/rtc_periph.h"
#include "driver/spi_slave.h"
#include "esp_log.h"
#include "esp_spi_flash.h"
#include "driver/gpio.h"

#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_now.h"
#include "esp_crc.h"
#include "espnow.h"
#include "spi.h"

static QueueHandle_t event_queue;

static const char *TAG = "EspEvent";

static void espV_task(void *pvParameter)
{
    event_t *event;
    
    while (1)
    {
        if (xQueueReceive(event_queue, &event, portMAX_DELAY) != pdTRUE)
            continue;
        switch (event->type)
        {
        case EVENT_TYPE_SPI_TIMER:
            spi_func();
            break;
        /*
        case EVENT_TYPE_NETWORK_CONNECTED:
            network_on_connected();
            break;
        case EVENT_TYPE_NETWORK_DISCONNECTED:
            network_on_disconnected();
            break;
        */    
        } 
        free(event);
    }
    vTaskDelete(NULL);
}


static void spi_timer_cb(TimerHandle_t xTimer)
{
    event_t *event = malloc(sizeof(*event));

    event->type = EVENT_TYPE_SPI_TIMER;

    ESP_LOGD(TAG, "Queuing event HEARTBEAT_TIMER");
    xQueueSend(event_queue, &event, portMAX_DELAY);
}

static int start_spi_task(void)
{
    TimerHandle_t spi_timer;

    if (!(event_queue = xQueueCreate(2, sizeof(event_t *))))
        return -1;
    if (xTaskCreatePinnedToCore(espV_task, "root_task", 4096, NULL, 5, NULL, 1) != pdPASS)
    {
        return -1;
    }

   /*
    if (xTaskCreatePinnedToCore(spi_task, "root_task", 4096, NULL, 5, NULL, 1) != pdPASS)
    {
        return -1;
    }
   */
    spi_timer = xTimerCreate("spi_timer", pdMS_TO_TICKS(20), pdTRUE,
        NULL, spi_timer_cb);
    xTimerStart(spi_timer, 0);

    return 0;
}


void app_main(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK( nvs_flash_erase() );
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );


    //example_wifi_init();
    //example_espnow_init();
    spi_init();
    start_spi_task();   
    
}