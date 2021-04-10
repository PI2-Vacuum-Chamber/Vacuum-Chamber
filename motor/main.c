	 #include <stdio.h>
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "esp_system.h"
    #include "driver/gpio.h"

    #define XDIR1_GPIO 35
	#define XDIR2_GPIO 34
    #define YDIR_GPIO 33
    #define PULL_GPIO 32
    #define START_GPIO 31
	int count = 0;

    void CONTROLE(void *pvParameter)
    {

        gpio_pad_select_gpio(XDIR1_GPIO);
        gpio_pad_select_gpio(XDIR2_GPIO);
        gpio_pad_select_gpio(YDIR_GPIO);
        gpio_pad_select_gpio(PULL_GPIO);
        gpio_pad_select_gpio(START_GPIO);

        gpio_set_direction(XDIR1_GPIO, GPIO_MODE_OUTPUT);
        gpio_set_direction(XDIR2_GPIO, GPIO_MODE_OUTPUT);
        gpio_set_direction(YDIR_GPIO, GPIO_MODE_OUTPUT);
        gpio_set_direction(PULL_GPIO, GPIO_MODE_OUTPUT);
        gpio_set_direction(START_GPIO, GPIO_MODE_INPUT);

        while(1) {
            if (START_GPIO == 0){
            	gpio_set_level(XDIR1_GPIO, 0);
            	gpio_set_level(XDIR2_GPIO, 0);
            	gpio_set_level(YDIR_GPIO, 0);
            	gpio_set_level(PULL_GPIO, 0);
            }

            if (START_GPIO == 1){

            	for(count = 101; count != 0; count--)
            	{
            		gpio_set_level(XDIR1_GPIO, 0);
            		gpio_set_level(XDIR2_GPIO, 1);
            		gpio_set_level(YDIR_GPIO, 0);
            		gpio_set_level(PULL_GPIO, 0);
            	vTaskDelay(50 / portTICK_RATE_MS);
            	gpio_set_level(PULL_GPIO, 1);
            	vTaskDelay(50 / portTICK_RATE_MS);
            	}
            	for(count = 101; count != 0; count--)
            	            	{
            	            		gpio_set_level(XDIR1_GPIO, 1);
            	            		gpio_set_level(XDIR2_GPIO, 0);
            	            		gpio_set_level(YDIR_GPIO, 0);
            	            		gpio_set_level(PULL_GPIO, 0);
            	            	vTaskDelay(50 / portTICK_RATE_MS);
            	            	gpio_set_level(PULL_GPIO, 1);
            	            	vTaskDelay(50 / portTICK_RATE_MS);
            	            	}
            	}
        }
    }


    void app_main()
    {
        nvs_flash_init();
        xTaskCreate(&CONTROLE, "CONTROLE", 2048, NULL, 5, NULL);
    }

