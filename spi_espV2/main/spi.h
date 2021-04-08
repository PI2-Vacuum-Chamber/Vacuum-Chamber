
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

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

#define GPIO_MOSI 12
#define GPIO_MISO 13
#define GPIO_SCLK 15
#define GPIO_CS 14
#define GPIO_HANDSHAKE 2

#ifdef CONFIG_IDF_TARGET_ESP32
#define RCV_HOST    HSPI_HOST
#define DMA_CHAN    2

#elif defined CONFIG_IDF_TARGET_ESP32S2
#define RCV_HOST    SPI2_HOST
#define DMA_CHAN    RCV_HOST

#endif

typedef enum {
    EVENT_TYPE_SPI_TIMER,
} event_type_t;

/*
typedef union {

} event_info_t;
*/
typedef struct {
    event_type_t type;
    //event_info_t info;
} event_t;

void my_post_setup_cb(spi_slave_transaction_t *trans);
void my_post_trans_cb(spi_slave_transaction_t *trans);
//void spi_task(void *pvParameter);
void spi_func();
void spi_init(void);