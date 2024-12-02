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
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_log.h"
#include "esp_system.h"
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

#define CONFIG_ENV_GPIO_RANGE_MIN 0
#define CONFIG_ENV_GPIO_RANGE_MAX 30
#define CONFIG_ENV_GPIO_IN_RANGE_MAX 30
#define CONFIG_ENV_GPIO_OUT_RANGE_MAX 30
#define CONFIG_BLINK_GPIO 8
#define CONFIG_BLINK_PERIOD 2000

/*##############################################################
 * TYPEDEFS
 *############################################################*/

typedef struct
{
    led_strip_handle_t handle;
    enum
    {
        OFF,
        RED,
        GREEN,
        BLUE,
        NUMBER_OF_LED_STATES
    } state;
} led_t;

/*##############################################################
 * CONSTANTS
 *############################################################*/

static const char *TAG = "ESP32-C6";

/*##############################################################
 * GLOBAL VARIABLES
 *############################################################*/

static led_t s_led;

/*##############################################################
 * FUNCTIONS
 *############################################################*/

static void blink_led(void)
{
    if (s_led.state == OFF)
    {
        ESP_LOGI(TAG, "Turning the LED \"OFF\"!");

        /* Set all LED off to clear all pixels */

        led_strip_clear(s_led.handle);
    }
    else if (s_led.state == RED)
    {
        ESP_LOGI(TAG, "Turning the LED \"RED\"!");

        /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for
         * each color. */

        led_strip_set_pixel(s_led.handle, 0, 16, 0, 0);

        /* Refresh the strip to send data. */

        led_strip_refresh(s_led.handle);
    }
    else if (s_led.state == GREEN)
    {
        ESP_LOGI(TAG, "Turning the LED \"GREEN\"!");
        led_strip_set_pixel(s_led.handle, 0, 0, 16, 0);
        led_strip_refresh(s_led.handle);
    }
    else if (s_led.state == BLUE)
    {
        ESP_LOGI(TAG, "Turning the LED \"BLUE\"!");
        led_strip_set_pixel(s_led.handle, 0, 0, 0, 16);
        led_strip_refresh(s_led.handle);
    }
    else
    {
        ESP_LOGE(TAG, "Invalid LED state.\n");
    }
}

static void configure_led(void)
{
    ESP_LOGI(TAG, "Configuring LED.\n");

    /* LED strip initialization with the GPIO and pixels number. */

    led_strip_config_t strip_config = {
        .strip_gpio_num = CONFIG_BLINK_GPIO,
        .max_leds = 1, /* At least one LED on board. */
    };
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, /* 10 MHz. */
        .flags.with_dma = false,
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &s_led.handle));

    /* Set all LED off to clear all pixels. */

    led_strip_clear(s_led.handle);
}

/*##############################################################
 * MAIN
 *############################################################*/

void app_main(void)
{
    printf("Booting up.\n");

    /* Print chip information. */

    esp_chip_info_t chip_info;
    uint32_t flash_size;
    esp_chip_info(&chip_info);
    printf("This is %s chip with:\n", CONFIG_IDF_TARGET);
    printf("\t- %d CPU core(s).\n", chip_info.cores);
    printf("\t- %s%s%s.\n",
           (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "Wi-Fi/" : "",
           (chip_info.features & CHIP_FEATURE_BT) ? "BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "");
    printf("\t- %s.\n",
           (chip_info.features & CHIP_FEATURE_IEEE802154) ? "802.15.4 (Zigbee/Thread)" : "");
    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;
    printf("\t- Silicon revision v%d.%d.\n", major_rev, minor_rev);
    if (esp_flash_get_size(NULL, &flash_size) != ESP_OK)
    {
        printf("Get flash size failed.");
        return;
    }
    printf("\t- %" PRIu32 " MB %s flash.\n", flash_size / (uint32_t)(1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
    printf("\t- Minimum free heap size: %" PRIu32 " bytes.\n", esp_get_minimum_free_heap_size());

    /* Configure the LED. */

    s_led.state = OFF;
    configure_led();

    /* Change the LED color. */

    while (1)
    {
        blink_led();
        s_led.state++;
        s_led.state %= NUMBER_OF_LED_STATES;
        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
    }
}
