/*##############################################################
 * FILE INFO
 *############################################################*/

/* Author: Travis Fredrickson. */
/* Date: 2024-12-01. */
/* Description: Does something on an ESP32-C6-DevKitC-1-N8. */

/*##############################################################
 * INCLUDES
 *############################################################*/

/*==============================================================
 * Standard includes.
 *============================================================*/

// #include <inttypes.h>
#include <stdio.h>

/*==============================================================
 * ESP includes.
 *============================================================*/

// #include "driver/gpio.h"
// #include "esp_chip_info.h"
// #include "esp_flash.h"
#include "esp_log.h"
// #include "esp_system.h"
#include "led_strip.h"
#include "sdkconfig.h"

/*==============================================================
 * FreeRTOS includes.
 *============================================================*/

// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"

/*##############################################################
 * DEFINES
 *############################################################*/

/*==============================================================
 * Config.
 *============================================================*/

#define CONFIG_ENV_GPIO_RANGE_MIN 0
#define CONFIG_ENV_GPIO_RANGE_MAX 30
#define CONFIG_ENV_GPIO_IN_RANGE_MAX 30
#define CONFIG_ENV_GPIO_OUT_RANGE_MAX 30
#define CONFIG_BLINK_GPIO 8
#define CONFIG_BLINK_PERIOD 2000

/*==============================================================
 * Other.
 *============================================================*/

#define BLINK_GPIO CONFIG_BLINK_GPIO

/*##############################################################
 * CONSTANTS
 *############################################################*/

static const char *TAG = "example";

static uint8_t s_led_state = 0;

/*##############################################################
 * GLOBAL VARIABLES
 *############################################################*/

/*##############################################################
 * FUNCTIONS
 *############################################################*/


static led_strip_handle_t led_strip;

static void blink_led(void)
{
    /* If the addressable LED is enabled */
    if (s_led_state) {
        /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
        led_strip_set_pixel(led_strip, 0, 16, 16, 16);
        /* Refresh the strip to send data */
        led_strip_refresh(led_strip);
    } else {
        /* Set all LED off to clear all pixels */
        led_strip_clear(led_strip);
    }
}

static void configure_led(void)
{
    ESP_LOGI(TAG, "Example configured to blink addressable LED!");
    /* LED strip initialization with the GPIO and pixels number*/
    led_strip_config_t strip_config = {
        .strip_gpio_num = BLINK_GPIO,
        .max_leds = 1, // at least one LED on board
    };
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
        .flags.with_dma = false,
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    /* Set all LED off to clear all pixels */
    led_strip_clear(led_strip);
}

/*##############################################################
 * MAIN
 *############################################################*/

void app_main(void)
{
    printf("Hello world!\n");

    /* Configure the peripheral according to the LED type. */

    configure_led();

    /* Toggle the LED. */

    while (1) {
        ESP_LOGI(TAG, "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
        blink_led();
        s_led_state = !s_led_state;
        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
    }
}
