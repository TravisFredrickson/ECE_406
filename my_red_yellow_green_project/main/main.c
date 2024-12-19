/*##############################################################
 * FILE INFO
 *############################################################*/

/* Author: Travis Fredrickson.
 * Date: 2024-12-19.
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
#include "led_strip.h"
#include "esp_system.h"
#include "esp_task_wdt.h"
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

#define ONE_SECOND 1000
#define STACK_DEPTH 2048
#define TAG "main"

/*==============================================================
 * LED strip.
 *============================================================*/

#define LED_STRIP_GPIO 8
#define LED_STRIP_GREEN_TASK_PRIORITY 4
#define LED_STRIP_YELLOW_TASK_PRIORITY 5
#define LED_STRIP_RED_TASK_PRIORITY 6

/*==============================================================
 * Button.
 *============================================================*/

#define BUTTON_GREEN_GPIO 4
#define BUTTON_YELLOW_GPIO 5
#define BUTTON_RED_GPIO 6
#define BUTTON_CHECK_TASK_PRIORITY 10

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
 * LED strip.
 *============================================================*/

static led_strip_t led_strip;
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
static void read_from_serial_port(void);

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

static void read_from_serial_port(void)
{
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
    /* Set all LED off to clear all pixels. */
    led_strip.state = OFF;
    led_strip_clear(led_strip.handle);
}

static void led_strip_update(void)
{
    switch (led_strip.state)
    {
    case RED:
        ESP_LOGI(TAG, "Turning the LED RED.");
        /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for
         * each color. */
        led_strip_set_pixel(led_strip.handle, 0, 16, 0, 0);
        /* Refresh the strip to send data. */
        led_strip_refresh(led_strip.handle);
        break;
    case YELLOW:
        ESP_LOGI(TAG, "Turning the LED YELLOW.");
        led_strip_set_pixel(led_strip.handle, 0, 16, 8, 0);
        led_strip_refresh(led_strip.handle);
        break;
    case GREEN:
        ESP_LOGI(TAG, "Turning the LED GREEN.");
        led_strip_set_pixel(led_strip.handle, 0, 0, 16, 0);
        led_strip_refresh(led_strip.handle);
        break;
    case BLUE:
        ESP_LOGI(TAG, "Turning the LED BLUE.");
        led_strip_set_pixel(led_strip.handle, 0, 0, 0, 16);
        led_strip_refresh(led_strip.handle);
        break;
    case OFF:
        ESP_LOGI(TAG, "Turning the LED OFF.");
        /* Set all LED off to clear all pixels. */
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
        led_strip.state = GREEN;
        led_strip_update();
        for (uint8_t i = 1; i <= 3; i++)
        {
            /* Wait with blocking. */
            // vTaskDelay(pdMS_TO_TICKS(ONE_SECOND));
            /* Wait without blocking. */
            TickType_t starting_tick = xTaskGetTickCount();
            TickType_t ticks_to_wait = pdMS_TO_TICKS(ONE_SECOND);
            while ((xTaskGetTickCount() - starting_tick) < ticks_to_wait)
            {
                /* Busy wait. */
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
        led_strip.state = YELLOW;
        led_strip_update();
        for (uint8_t i = 1; i <= 3; i++)
        {
            /* Wait without blocking. */
            TickType_t starting_tick = xTaskGetTickCount();
            TickType_t ticks_to_wait = pdMS_TO_TICKS(ONE_SECOND);
            while ((xTaskGetTickCount() - starting_tick) < ticks_to_wait)
            {
                /* Busy wait. */
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
        led_strip.state = RED;
        led_strip_update();
        for (uint8_t i = 1; i <= 3; i++)
        {
            /* Wait without blocking. */
            TickType_t starting_tick = xTaskGetTickCount();
            TickType_t ticks_to_wait = pdMS_TO_TICKS(ONE_SECOND);
            while ((xTaskGetTickCount() - starting_tick) < ticks_to_wait)
            {
                /* Busy wait. */
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
        if (xQueueReceive(button_events, &button_event, pdMS_TO_TICKS(ONE_SECOND)))
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
        vTaskDelay(pdMS_TO_TICKS(ONE_SECOND / 100));
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
    led_strip_configure();
    button_configure();

    /* Create tasks. */
    xTaskCreate(
        &led_strip_green_task,
        "led_strip_green_task",
        STACK_DEPTH,
        NULL,
        LED_STRIP_GREEN_TASK_PRIORITY,
        &led_strip_green_task_handle);
    xTaskCreate(
        &led_strip_yellow_task,
        "led_strip_yellow_task",
        STACK_DEPTH,
        NULL,
        LED_STRIP_YELLOW_TASK_PRIORITY,
        &led_strip_yellow_task_handle);
    xTaskCreate(
        &led_strip_red_task,
        "led_strip_red_task",
        STACK_DEPTH,
        NULL,
        LED_STRIP_RED_TASK_PRIORITY,
        &led_strip_red_task_handle);
    xTaskCreate(
        &button_check_task,
        "button_check_task",
        STACK_DEPTH,
        NULL,
        BUTTON_CHECK_TASK_PRIORITY,
        &button_check_task_handle);

    /* Start scheduler. */
    // vTaskStartScheduler();

    /* Print task information. */
    ESP_LOGI(TAG, "uxTaskPriorityGet(led_strip_green_task_handle) = %u.",
             uxTaskPriorityGet(led_strip_green_task_handle));
    ESP_LOGI(TAG, "uxTaskPriorityGet(led_strip_yellow_task_handle) = %u.",
             uxTaskPriorityGet(led_strip_yellow_task_handle));
    ESP_LOGI(TAG, "uxTaskPriorityGet(led_strip_red_task_handle) = %u.",
             uxTaskPriorityGet(led_strip_red_task_handle));
    ESP_LOGI(TAG, "uxTaskPriorityGet(button_check_task_handle) = %u.",
             uxTaskPriorityGet(button_check_task_handle));
}
