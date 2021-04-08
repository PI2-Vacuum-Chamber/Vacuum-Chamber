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

#include "spi.h"


static const char *TAG2 = "spi";
esp_err_t ret_spi;


//Called after a transaction is queued and ready for pickup by master. We use this to set the handshake line high.
void my_post_setup_cb(spi_slave_transaction_t *trans) {
    WRITE_PERI_REG(GPIO_OUT_W1TS_REG, (1<<GPIO_HANDSHAKE));
}

//Called after transaction is sent/received. We use this to set the handshake line low.
void my_post_trans_cb(spi_slave_transaction_t *trans) {
    WRITE_PERI_REG(GPIO_OUT_W1TC_REG, (1<<GPIO_HANDSHAKE));
}

//Configuration for the SPI bus
    spi_bus_config_t buscfg={
        .mosi_io_num=GPIO_MOSI,
        .miso_io_num=GPIO_MISO,
        .sclk_io_num=GPIO_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };
    //Configuration for the SPI slave interface
    spi_slave_interface_config_t slvcfg={
        .mode=0,
        .spics_io_num=GPIO_CS,
        .queue_size=3,
        .flags=0,
        .post_setup_cb=my_post_setup_cb,
        .post_trans_cb=my_post_trans_cb
    };

    //Configuration for the handshake line
    gpio_config_t io_conf={
        .intr_type=GPIO_INTR_DISABLE,
        .mode=GPIO_MODE_OUTPUT,
        .pin_bit_mask=(1<<GPIO_HANDSHAKE)
    };
   

void spi_init(void){
    //Configure handshake line as output
    gpio_config(&io_conf);
    //Enable pull-ups on SPI lines so we don't detect rogue pulses when no master is connected.
    gpio_set_pull_mode(GPIO_MOSI, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(GPIO_SCLK, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(GPIO_CS, GPIO_PULLUP_ONLY);

    //Initialize SPI slave interface
    ret_spi=spi_slave_initialize(RCV_HOST, &buscfg, &slvcfg, DMA_CHAN);
    assert(ret_spi==ESP_OK);
}
//void spi_task(void *pvParameter)
void spi_func()
{   
    /*
    //Configuration for the SPI bus
    spi_bus_config_t buscfg={
        .mosi_io_num=GPIO_MOSI,
        .miso_io_num=GPIO_MISO,
        .sclk_io_num=GPIO_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };
    //Configuration for the SPI slave interface
    spi_slave_interface_config_t slvcfg={
        .mode=0,
        .spics_io_num=GPIO_CS,
        .queue_size=3,
        .flags=0,
        .post_setup_cb=my_post_setup_cb,
        .post_trans_cb=my_post_trans_cb
    };

    //Configuration for the handshake line
    gpio_config_t io_conf={
        .intr_type=GPIO_INTR_DISABLE,
        .mode=GPIO_MODE_OUTPUT,
        .pin_bit_mask=(1<<GPIO_HANDSHAKE)
    };
   

    //Configure handshake line as output
    gpio_config(&io_conf);
    //Enable pull-ups on SPI lines so we don't detect rogue pulses when no master is connected.
    gpio_set_pull_mode(GPIO_MOSI, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(GPIO_SCLK, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(GPIO_CS, GPIO_PULLUP_ONLY);

    //Initialize SPI slave interface
    ret_spi=spi_slave_initialize(RCV_HOST, &buscfg, &slvcfg, DMA_CHAN);
    assert(ret_spi==ESP_OK);
    */

    WORD_ALIGNED_ATTR char sendbuf2[14] = ""; //129
    //WORD_ALIGNED_ATTR unsigned long recvbuf2 = 0;
    WORD_ALIGNED_ATTR char recvbuf3[14] = "";
    WORD_ALIGNED_ATTR double data = 0.0;

    //MDF_LOGI("Spi is running");

    for (int i = 0;; ++i) {
        
        spi_slave_transaction_t t;
        memset(&t, 0, sizeof(t));

        //Clear receive buffer, set send buffer to something sane
        //recvbuf2 = 0;
        //memset(recvbuf2, 0xA5, 129);

        memset(recvbuf3, 0xA5, 14);
        
        if (i<14)
        {
            sprintf(sendbuf2, "Recebido(esp)");
        }
        
        //Set up a transaction of 128 bytes to send/receive
        t.length=128*8;
        t.tx_buffer=sendbuf2;//sendbuf;
        //t.rx_buffer=&recvbuf2;
        t.rx_buffer=recvbuf3;
        //                  T clkmaster = 0.2 us
        
        ret_spi=spi_slave_transmit(RCV_HOST, &t, portMAX_DELAY);

        ESP_LOGI(TAG2, "Temperatura[%d] rx buffer: %s",i,recvbuf3);
        sscanf(recvbuf3, "%lf", &data);

        ESP_LOGI(TAG2, "tx buffer: %s", sendbuf2);
        printf("\n");
        if (i==13)
        {
            i=0;
            break;
        }
        memset(sendbuf2, 0xA5, 14);
    }
    //return recvbuf3;
    //vTaskDelete(NULL);
}
