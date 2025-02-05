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
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_log.h"
#include "led_strip.h"
#include "esp_task_wdt.h"
#include "sdkconfig.h"

/*==============================================================
 * FreeRTOS.
 *============================================================*/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/*==============================================================
 * Button.
 *============================================================*/

#include "./button/include/button.h"

/*==============================================================
 * Zigbee.
 *============================================================*/

#include "./zigbee/include/esp_zb_switch.h"

/*##############################################################
 * DEFINES
 *############################################################*/

/*==============================================================
 * General.
 *============================================================*/

#define TAG "main"
#define TASK_STACK_DEPTH 4096

/*==============================================================
 * Button.
 *============================================================*/

#define BUTTON_GREEN_GPIO 4
#define BUTTON_YELLOW_GPIO 5
#define BUTTON_RED_GPIO 6
#define BUTTON_CHECK_TASK_PRIORITY 10

/*==============================================================
 * LED strip.
 *============================================================*/

#define LED_STRIP_GPIO 8
#define LED_STRIP_GREEN_TASK_PRIORITY 4
#define LED_STRIP_YELLOW_TASK_PRIORITY 5
#define LED_STRIP_RED_TASK_PRIORITY 6
#define LED_STRIP_TASK_SECONDS 3

/*##############################################################
 * TYPEDEFS
 *############################################################*/

typedef struct
{
    led_strip_handle_t handle;
    enum
    {
        OFF = 0,
        RED,
        YELLOW,
        GREEN,
        BLUE,
        LED_STRIP_STATE_COUNT
    } state;
} led_strip_t;

/*##############################################################
 * CONSTANTS
 *############################################################*/

/*##############################################################
 * GLOBAL VARIABLES
 *############################################################*/

/*==============================================================
 * Button.
 *============================================================*/

static button_event_t button_event;
static QueueHandle_t button_events;
static TaskHandle_t button_check_task_handle = NULL;

/*==============================================================
 * LED strip.
 *============================================================*/

static led_strip_t led_strip;
static TaskHandle_t led_strip_green_task_handle = NULL;
static TaskHandle_t led_strip_red_task_handle = NULL;
static TaskHandle_t led_strip_yellow_task_handle = NULL;

/*##############################################################
 * FUNCTION PROTOTYPES
 *############################################################*/

/*==============================================================
 * General.
 *============================================================*/

static void print_chip_information(void);

/*==============================================================
 * Button.
 *============================================================*/

static void button_configure(void);
static void button_check_task(void *pvParameter);

/*==============================================================
 * LED strip.
 *============================================================*/

static void led_strip_configure(void);
static void led_strip_update(void);
static void led_strip_green_task(void *pvParameter);
static void led_strip_yellow_task(void *pvParameter);
static void led_strip_red_task(void *pvParameter);

/*##############################################################
 * FUNCTIONS
 *############################################################*/

/*==============================================================
 * General.
 *============================================================*/

static void print_chip_information(void)
{
    esp_chip_info_t chip_info;
    uint32_t flash_size;
    esp_chip_info(&chip_info);
    printf("This is %s chip with:\n", CONFIG_IDF_TARGET);
    printf("\t- %u CPU core(s).\n", chip_info.cores);
    printf("\t- %s%s%s.\n",
           (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "Wi-Fi/" : "",
           (chip_info.features & CHIP_FEATURE_BT) ? "BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "");
    printf("\t- %s.\n",
           (chip_info.features & CHIP_FEATURE_IEEE802154) ? "802.15.4 (Zigbee/Thread)" : "");
    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;
    printf("\t- Silicon revision v%u.%u.\n", major_rev, minor_rev);
    if (esp_flash_get_size(NULL, &flash_size) != ESP_OK)
    {
        printf("Get flash size failed.");
        return;
    }
    printf("\t- %" PRIu32 " MB %s flash.\n", flash_size / (uint32_t)(1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
    printf("\t- Minimum free heap size: %" PRIu32 " bytes.\n", esp_get_minimum_free_heap_size());
}

/*==============================================================
 * Button.
 *============================================================*/

static void button_configure(void)
{
    ESP_LOGI(TAG, "button_configure().");
    button_events = pulled_button_init(
        (PIN_BIT(BUTTON_GREEN_GPIO) | PIN_BIT(BUTTON_YELLOW_GPIO) | PIN_BIT(BUTTON_RED_GPIO)),
        GPIO_PULLUP_ONLY);
}

static void button_check_task(void *pvParameter)
{
    for (;;)
    {
        if (xQueueReceive(button_events, &button_event, pdMS_TO_TICKS(1000)))
        {
            if ((button_event.pin == BUTTON_GREEN_GPIO) && (button_event.event == BUTTON_DOWN))
            {
                vTaskResume(led_strip_green_task_handle);
            }
            if ((button_event.pin == BUTTON_YELLOW_GPIO) && (button_event.event == BUTTON_DOWN))
            {
                vTaskResume(led_strip_yellow_task_handle);
            }
            if ((button_event.pin == BUTTON_RED_GPIO) && (button_event.event == BUTTON_DOWN))
            {
                vTaskResume(led_strip_red_task_handle);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    vTaskDelete(NULL);
}

/*==============================================================
 * LED strip.
 *============================================================*/

static void led_strip_configure(void)
{
    ESP_LOGI(TAG, "led_strip_configure().");
    /* LED strip initialization with the GPIO and pixels number. */
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_STRIP_GPIO,
        .max_leds = 1, /* At least one LED on board. */
    };
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, /* 10 MHz. */
        .flags.with_dma = false,
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip.handle));
    /* Turn the LED off. */
    led_strip.state = OFF;
    led_strip_clear(led_strip.handle);
}

static void led_strip_update(void)
{
    switch (led_strip.state)
    {
    case RED:
        ESP_LOGI(TAG, "Turning the LED red.");
        /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for
         * each color. */
        led_strip_set_pixel(led_strip.handle, 0, 16, 0, 0);
        /* Refresh the strip to send data. */
        led_strip_refresh(led_strip.handle);
        break;
    case YELLOW:
        ESP_LOGI(TAG, "Turning the LED yellow.");
        led_strip_set_pixel(led_strip.handle, 0, 16, 8, 0);
        led_strip_refresh(led_strip.handle);
        break;
    case GREEN:
        ESP_LOGI(TAG, "Turning the LED green.");
        led_strip_set_pixel(led_strip.handle, 0, 0, 16, 0);
        led_strip_refresh(led_strip.handle);
        break;
    case BLUE:
        ESP_LOGI(TAG, "Turning the LED blue.");
        led_strip_set_pixel(led_strip.handle, 0, 0, 0, 16);
        led_strip_refresh(led_strip.handle);
        break;
    case OFF:
        ESP_LOGI(TAG, "Turning the LED off.");
        led_strip_clear(led_strip.handle);
        break;
    default:
        ESP_LOGE(TAG, "Invalid LED state.\n");
        led_strip_clear(led_strip.handle);
        break;
    }
}

static void led_strip_green_task(void *pvParameter)
{
    /* Wait for task to be called via vTaskResume(). */
    vTaskSuspend(NULL);
    for (;;)
    {
        ESP_LOGI(TAG, "Beginning led_strip_green_task().");
        for (uint8_t i = 1; i <= LED_STRIP_TASK_SECONDS; i++)
        {
            /* Wait by blocking. */
            // vTaskDelay(pdMS_TO_TICKS(1000));
            /* Wait by busy waiting. */
            TickType_t starting_tick = xTaskGetTickCount();
            TickType_t ticks_to_wait = pdMS_TO_TICKS(1000);
            while ((xTaskGetTickCount() - starting_tick) < ticks_to_wait)
            {
                /* Busy wait. */
                if (led_strip.state != GREEN)
                {
                    led_strip.state = GREEN;
                    led_strip_update();
                }
            }
            ESP_LOGI(TAG, "led_strip_green_task(): %u second.", i);
        }
        ESP_LOGI(TAG, "Ending led_strip_green_task().");
        led_strip.state = OFF;
        led_strip_update();
        vTaskSuspend(NULL);
    }
    vTaskDelete(NULL);
}

static void led_strip_yellow_task(void *pvParameter)
{
    /* Wait for task to be called via vTaskResume(). */
    vTaskSuspend(NULL);
    for (;;)
    {
        ESP_LOGW(TAG, "Beginning led_strip_yellow_task().");
        for (uint8_t i = 1; i <= LED_STRIP_TASK_SECONDS; i++)
        {
            /* Wait by busy waiting. */
            TickType_t starting_tick = xTaskGetTickCount();
            TickType_t ticks_to_wait = pdMS_TO_TICKS(1000);
            while ((xTaskGetTickCount() - starting_tick) < ticks_to_wait)
            {
                /* Busy wait. */
                if (led_strip.state != YELLOW)
                {
                    led_strip.state = YELLOW;
                    led_strip_update();
                }
            }
            ESP_LOGW(TAG, "led_strip_yellow_task(): %u second.", i);
        }
        ESP_LOGW(TAG, "Ending led_strip_yellow_task().");
        led_strip.state = OFF;
        led_strip_update();
        vTaskSuspend(NULL);
    }
    vTaskDelete(NULL);
}

static void led_strip_red_task(void *pvParameter)
{
    /* Wait for task to be called via vTaskResume(). */
    vTaskSuspend(NULL);
    for (;;)
    {
        ESP_LOGE(TAG, "Beginning led_strip_red_task().");
        for (uint8_t i = 1; i <= LED_STRIP_TASK_SECONDS; i++)
        {
            /* Wait by busy waiting. */
            TickType_t starting_tick = xTaskGetTickCount();
            TickType_t ticks_to_wait = pdMS_TO_TICKS(1000);
            while ((xTaskGetTickCount() - starting_tick) < ticks_to_wait)
            {
                /* Busy wait. */
                if (led_strip.state != RED)
                {
                    led_strip.state = RED;
                    led_strip_update();
                }
            }
            ESP_LOGE(TAG, "led_strip_red_task(): %u second.", i);
        }
        ESP_LOGE(TAG, "Ending led_strip_red_task().");
        led_strip.state = OFF;
        led_strip_update();
        vTaskSuspend(NULL);
    }
    vTaskDelete(NULL);
}

/*##############################################################
 * MAIN
 *############################################################*/

void app_main(void)
{
    /* Initalize and configure. */
    print_chip_information();
    button_configure();
    // zigbee_configure();
    led_strip_configure();

    /* Create tasks. */
    xTaskCreate(
        &led_strip_green_task,
        "led_strip_green_task",
        TASK_STACK_DEPTH,
        NULL,
        LED_STRIP_GREEN_TASK_PRIORITY,
        &led_strip_green_task_handle);
    xTaskCreate(
        &led_strip_yellow_task,
        "led_strip_yellow_task",
        TASK_STACK_DEPTH,
        NULL,
        LED_STRIP_YELLOW_TASK_PRIORITY,
        &led_strip_yellow_task_handle);
    xTaskCreate(
        &led_strip_red_task,
        "led_strip_red_task",
        TASK_STACK_DEPTH,
        NULL,
        LED_STRIP_RED_TASK_PRIORITY,
        &led_strip_red_task_handle);
    xTaskCreate(
        &button_check_task,
        "button_check_task",
        TASK_STACK_DEPTH,
        NULL,
        BUTTON_CHECK_TASK_PRIORITY,
        &button_check_task_handle);

    /* Turn the LED off. The Zigbee code turns it bright green for
     * an unkown reason. This may not work as it may be delayed.
     * Seems to work though, the LED briefly turns green, then turns
     * off. */
    led_strip.state = OFF;
    led_strip_clear(led_strip.handle);
}
