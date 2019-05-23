#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "driver/i2s.h"

#include "nvs_flash.h"
#include "nvs.h"



#include "esp_wifi.h"

#include "esp_system.h"
#include "esp_err.h"
#include "esp_log.h"
#include "../lvgl/lv_core/lv_group.h"
#include "../lvgl/lv_core/lv_indev.h"



#include "keypad_control.h"


/*
#define ADCCHANNEL ADC1_CHANNEL_6

lv_indev_t *keypad_indev;
static uint32_t last_key;
static lv_indev_state_t state;
QueueHandle_t Queue_Key;

static const char* TAG = "KEYPAD";

key_event_t keyEvent;
TickType_t key_last_tick;
*/
#define GPIO_INPUT_IO_5     23
#define GPIO_INPUT_IO_4     27
#define GPIO_INPUT_IO_3     32
#define GPIO_INPUT_IO_2     33
#define GPIO_INPUT_IO_1     34
#define GPIO_INPUT_IO_0     35
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_IO_0) | (1ULL<<GPIO_INPUT_IO_1))
//| (1ULL<<GPIO_INPUT_IO_2)| (1ULL<<GPIO_INPUT_IO_3)| (1ULL<<GPIO_INPUT_IO_4)| (1ULL<<GPIO_INPUT_IO_5))
#define ESP_INTR_FLAG_DEFAULT 0

uint8_t keymode = 0 ;
//TaskHandle_t keyHandle1 = NULL;

esp_err_t nvs_erase_wifi_data(void)
{
    nvs_handle my_handle;
    esp_err_t err;

    // Open
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;

    
    err = nvs_erase_key(my_handle, "wssid");
    if (err != ESP_OK) return err;
	
	err = nvs_erase_key(my_handle, "wpass");
    if (err != ESP_OK) return err;

    // Close
    nvs_close(my_handle);
    return ESP_OK;
}


void taskScanKey(void *patameter) {


	esp_err_t err = ESP_OK;
	uint8_t key0num = 0 ;
	gpio_config_t io_conf;
    /*//disable interrupt
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
*/
    //interrupt of rising edge        		    GPIO_PIN_INTR_POSEDGE
    io_conf.intr_type =GPIO_PIN_INTR_DISABLE ;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //set as input mode    
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
	io_conf.pull_down_en = 0 ;
    gpio_config(&io_conf);

	uint32_t io_num;
    for(;;) {
        if(gpio_get_level(GPIO_INPUT_IO_0)==0) {
			vTaskDelay(10 / portTICK_RATE_MS);
			if(gpio_get_level(GPIO_INPUT_IO_0)==0) {
            	printf("GPIO[%d] intr, val 1: \n", GPIO_INPUT_IO_0);
				key0num ++ ;
				if(key0num >100 )
				{
					key0num = 0 ; 
					printf("--------------------------");
					 ESP_ERROR_CHECK( esp_wifi_disconnect() );
					 vTaskDelay(1000/ portTICK_RATE_MS);
					//ESP_ERROR_CHECK(nvs_flash_erase());
					if( ESP_OK== nvs_erase_wifi_data() )
					{
						printf("!!!!!erasewifi ssid password success!!!!!!!!!!!!!");
					}
					else
					{
						printf("!!!!!erasewifi ssid password faile123+++!!!!!!!!!!!!!");
					}
					printf("++++++++++++++++++++++++++");
				}
				vTaskDelay(10 / portTICK_RATE_MS);
			}
        }
		vTaskDelay(10 / portTICK_RATE_MS);
/*
		if(gpio_get_level(GPIO_INPUT_IO_1)==1) {
			vTaskDelay(10 / portTICK_RATE_MS);
			if(gpio_get_level(GPIO_INPUT_IO_1)==1) {
            	printf("GPIO[%d] intr, val 1: \n", GPIO_INPUT_IO_1);
				
				vTaskDelay(10 / portTICK_RATE_MS);
				}
        }
*/
	}
		

}

/*
esp_err_t keyQueueCreate() {
	key_last_tick = xTaskGetTickCount();
	Queue_Key = xQueueCreate(16, sizeof(key_event_t));
	if(Queue_Key == 0) return ESP_FAIL;

	return ESP_OK;
}

bool keypad_read(lv_indev_data_t *data) {
	key_event_t key_event;
	if(xQueueReceive(Queue_Key, &key_event, 10) == pdPASS) {
		data->state = key_event.state;
		data->key = key_event.key_name;
	} else {
		data->state = state;
		data->key = last_key;
	}

	return false;
}
*/
