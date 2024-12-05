/*##############################################################
 * FILE INFO
 *############################################################*/

/* Author: Travis Fredrickson.
 * Date: 2024-12-01.
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

#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_log.h"
#include "esp_system.h"
#include "led_strip.h"
#include "sdkconfig.h"

/*==============================================================
 * FreeRTOS.
 *============================================================*/

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

/*==============================================================
 * User.
 *============================================================*/

#include "button.h"

/*##############################################################
 * DEFINES
 *############################################################*/

/*==============================================================
 * General.
 *============================================================*/

#define DELAY_PERIOD_MS 1000
#define TAG "MAIN.C"

/*==============================================================
 * LED strip.
 *============================================================*/

#define LED_STRIP_GPIO 8

/*##############################################################
 * TYPEDEFS
 *############################################################*/

/* This could be in a separate file for better OOP. */
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
        LED_STATE_COUNT
    } state;
} led_strip_t;

/*##############################################################
 * CONSTANTS
 *############################################################*/

/*##############################################################
 * GLOBAL VARIABLES
 *############################################################*/

/*==============================================================
 * LED strip.
 *============================================================*/

static led_strip_t s_led_strip;
static TaskHandle_t led_strip_green_task_handle = NULL;
static TaskHandle_t led_strip_red_task_handle = NULL;
static TaskHandle_t led_strip_yellow_task_handle = NULL;

/*==============================================================
 * Button.
 *============================================================*/

static button_event_t button_event;
static QueueHandle_t button_events;
static TaskHandle_t button_check_task_handle = NULL;

/*##############################################################
 * FUNCTION PROTOTYPES
 *############################################################*/

/*==============================================================
 * General.
 *============================================================*/

static void print_chip_information(void);

/*==============================================================
 * LED strip.
 *============================================================*/

static void led_strip_configure(void);
static void led_strip_update(void);
static void led_strip_green_task(void *pvParameter);
static void led_strip_yellow_task(void *pvParameter);
static void led_strip_red_task(void *pvParameter);

/*==============================================================
 * Button.
 *============================================================*/

static void button_configure(void);
static void button_check_task(void *pvParameter);

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
}

/*==============================================================
 * LED strip.
 *============================================================*/

static void led_strip_configure(void)
{
    ESP_LOGI(TAG, "Configuring LED strip.\n");
    /* LED strip initialization with the GPIO and pixels number. */
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_STRIP_GPIO,
        .max_leds = 1, /* At least one LED on board. */
    };
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, /* 10 MHz. */
        .flags.with_dma = false,
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &s_led_strip.handle));
    /* Set all LED off to clear all pixels. */
    s_led_strip.state = OFF;
    led_strip_clear(s_led_strip.handle);
}

static void led_strip_update(void)
{
    switch (s_led_strip.state)
    {
    case RED:
        ESP_LOGI(TAG, "Turning the LED RED.");
        /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for
         * each color. */
        led_strip_set_pixel(s_led_strip.handle, 0, 16, 0, 0);
        /* Refresh the strip to send data. */
        led_strip_refresh(s_led_strip.handle);
        break;
    case YELLOW:
        ESP_LOGI(TAG, "Turning the LED YELLOW.");
        led_strip_set_pixel(s_led_strip.handle, 0, 16, 8, 0);
        led_strip_refresh(s_led_strip.handle);
        break;
    case GREEN:
        ESP_LOGI(TAG, "Turning the LED GREEN.");
        led_strip_set_pixel(s_led_strip.handle, 0, 0, 16, 0);
        led_strip_refresh(s_led_strip.handle);
        break;
    case BLUE:
        ESP_LOGI(TAG, "Turning the LED BLUE.");
        led_strip_set_pixel(s_led_strip.handle, 0, 0, 0, 16);
        led_strip_refresh(s_led_strip.handle);
        break;
    case OFF:
        ESP_LOGI(TAG, "Turning the LED OFF.");
        /* Set all LED off to clear all pixels. */
        led_strip_clear(s_led_strip.handle);
        break;
    default:
        ESP_LOGE(TAG, "Invalid LED state.\n");
        led_strip_clear(s_led_strip.handle);
        break;
    }
}

static void led_strip_green_task(void *pvParameter)
{
    for (;;)
    {
        ESP_LOGI(TAG, "Beginning led_strip_green_task.");
        s_led_strip.state = GREEN;
        led_strip_update();
        vTaskDelay(pdMS_TO_TICKS(5 * DELAY_PERIOD_MS));
        ESP_LOGI(TAG, "Ending led_strip_green_task.");
        s_led_strip.state = OFF;
        led_strip_update();
        vTaskSuspend(NULL);
    }
}

static void led_strip_yellow_task(void *pvParameter)
{
    for (;;)
    {
        ESP_LOGI(TAG, "Beginning led_strip_yellow_task.");
        s_led_strip.state = YELLOW;
        led_strip_update();
        vTaskDelay(pdMS_TO_TICKS(5 * DELAY_PERIOD_MS));
        ESP_LOGI(TAG, "Ending led_strip_yellow_task.");
        s_led_strip.state = OFF;
        led_strip_update();
        vTaskSuspend(NULL);
    }
}

static void led_strip_red_task(void *pvParameter)
{
    for (;;)
    {
        ESP_LOGI(TAG, "Beginning led_strip_red_task.");
        s_led_strip.state = RED;
        led_strip_update();
        vTaskDelay(pdMS_TO_TICKS(5 * DELAY_PERIOD_MS));
        ESP_LOGI(TAG, "Ending led_strip_red_task.");
        s_led_strip.state = OFF;
        led_strip_update();
        vTaskSuspend(NULL);
    }
}

/*==============================================================
 * Button.
 *============================================================*/

static void button_configure(void)
{
    button_events = pulled_button_init(
        (PIN_BIT(4) | PIN_BIT(5) | PIN_BIT(6)), GPIO_PULLUP_ONLY);
}

static void button_check_task(void *pvParameter)
{
    for (;;)
    {
        if (xQueueReceive(button_events, &button_event, pdMS_TO_TICKS(DELAY_PERIOD_MS)))
        {
            if ((button_event.pin == 4) && (button_event.event == BUTTON_DOWN))
            {
                vTaskResume(led_strip_green_task_handle);
            }
            if ((button_event.pin == 5) && (button_event.event == BUTTON_DOWN))
            {
                vTaskResume(led_strip_yellow_task_handle);
            }
            if ((button_event.pin == 6) && (button_event.event == BUTTON_DOWN))
            {
                vTaskResume(led_strip_red_task_handle);
            }
        }
    }
    vTaskDelay(pdMS_TO_TICKS(DELAY_PERIOD_MS / 10));
}

/*##############################################################
 * MAIN
 *############################################################*/

void app_main(void)
{
    /* Initalize and configure. */
    print_chip_information();
    led_strip_configure();
    button_configure();

    /* Create tasks. */
    xTaskCreate(&button_check_task, "button_check_task", 2048, NULL, 10, &button_check_task_handle);
    xTaskCreate(&led_strip_green_task, "led_strip_green_task", 2048, NULL, 4, &led_strip_green_task_handle);
    xTaskCreate(&led_strip_yellow_task, "led_strip_yellow_task", 2048, NULL, 5, &led_strip_yellow_task_handle);
    xTaskCreate(&led_strip_red_task, "led_strip_red_task", 2048, NULL, 6, &led_strip_red_task_handle);

    /* Suspend some tasks. */
    vTaskSuspend(led_strip_green_task_handle);
    vTaskSuspend(led_strip_yellow_task_handle);
    vTaskSuspend(led_strip_red_task_handle);

    /* Print task information. */
    printf("uxTaskPriorityGet(button_check_task_handle) = %d.\n", uxTaskPriorityGet(button_check_task_handle));
    printf("uxTaskPriorityGet(led_strip_green_task_handle) = %d.\n", uxTaskPriorityGet(led_strip_green_task_handle));
    printf("uxTaskPriorityGet(led_strip_yellow_task_handle) = %d.\n", uxTaskPriorityGet(led_strip_yellow_task_handle));
    printf("uxTaskPriorityGet(led_strip_red_task_handle) = %d.\n", uxTaskPriorityGet(led_strip_red_task_handle));
}
