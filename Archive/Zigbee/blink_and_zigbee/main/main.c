/*##############################################################
 * FILE INFO
 *############################################################*/

/* Author: Travis Fredrickson.
 * Date: 2025-02-04.
 * Description: Does something on an ESP32-C6-DevKitC-1-N8. */

/*##############################################################
 * INCLUDES
 *############################################################*/

/*==============================================================
 * Standard.
 *============================================================*/

#include <stdio.h>

/*==============================================================
 * ESP.
 *============================================================*/

#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"

/*==============================================================
 * FreeRTOS.
 *============================================================*/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/*==============================================================
 * Zigbee.
 *============================================================*/

#include "./zigbee/esp_zb_switch.h"

/*##############################################################
 * DEFINES
 *############################################################*/

#define BLINK_GPIO 8
#define BLINK_PERIOD_MS 1000

/*##############################################################
 * TYPEDEFS
 *############################################################*/

/*##############################################################
 * CONSTANTS
 *############################################################*/

static const char *TAG = "example";

/*##############################################################
 * GLOBAL VARIABLES
 *############################################################*/

static led_strip_handle_t led_strip;
static uint8_t s_led_state = 0;

/*##############################################################
 * FUNCTION PROTOTYPES
 *############################################################*/

/*##############################################################
 * FUNCTIONS
 *############################################################*/

/*==============================================================
 * LED.
 *============================================================*/

static void blink_led(void)
{
    /* If the addressable LED is enabled */
    if (s_led_state)
    {
        /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
        led_strip_set_pixel(led_strip, 0, 8, 8, 8);
        /* Refresh the strip to send data */
        led_strip_refresh(led_strip);
    }
    else
    {
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
    configure_led();
    configure_zigbee();

    while (1)
    {
        ESP_LOGI(TAG, "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
        blink_led();
        /* Toggle the LED state */
        s_led_state = !s_led_state;
        vTaskDelay(BLINK_PERIOD_MS / portTICK_PERIOD_MS);
    }
}
