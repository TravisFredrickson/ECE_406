/*##############################################################
 * FILE INFO
 *############################################################*/

/* Author: Travis Fredrickson.
 * Date: 2025-03-13.
 * Description: Does something on an ESP32-C6-DevKitC-1-N8. */

/*##############################################################
 * INCLUDES
 *############################################################*/

/*==============================================================
 * Standard.
 *============================================================*/

#include <stdio.h>
#include <string.h>

/*==============================================================
 * ESP.
 *============================================================*/

#include "driver/gpio.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_log.h"
#include "esp_task_wdt.h"
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

#include "./zigbee/include/esp_zb_switch.h"

/*##############################################################
 * DEFINES
 *############################################################*/

/*==============================================================
 * General.
 *============================================================*/

#define TAG "main"
#define TASK_STACK_DEPTH 4096
#define RYG_TASK_SECONDS 3
#define RED_TASK_PRIORITY configMAX_PRIORITIES - 4
#define YELLOW_TASK_PRIORITY configMAX_PRIORITIES - 5
#define GREEN_TASK_PRIORITY configMAX_PRIORITIES - 6

/*==============================================================
 * LED strip.
 *============================================================*/

#define LED_STRIP_GPIO GPIO_NUM_8

/*==============================================================
 * UART.
 *============================================================*/

#define UART_RX_BUFFER_SIZE 1024
#define UART_RX_PIN GPIO_NUM_17
#define UART_RX_TASK_PRIORITY configMAX_PRIORITIES - 1
#define UART_TX_PIN GPIO_NUM_4
#define UART_TX_TASK_PRIORITY configMAX_PRIORITIES - 2

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

const bool is_debug_on = false;

/*##############################################################
 * GLOBAL VARIABLES
 *############################################################*/

/*==============================================================
 * General.
 *============================================================*/

static TaskHandle_t red_task_handle = NULL;
static TaskHandle_t yellow_task_handle = NULL;
static TaskHandle_t green_task_handle = NULL;


/*==============================================================
 * LED strip.
 *============================================================*/

static led_strip_t led_strip;

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
static void red_task(void *pvParameter);
static void yellow_task(void *pvParameter);
static void green_task(void *pvParameter);

/*==============================================================
 * UART.
 *============================================================*/

static void uart_configure(void);
static void uart_rx_task(void *arg);

/*##############################################################
 * FUNCTIONS
 *############################################################*/

/*==============================================================
 * General.
 *============================================================*/

/*--------------------------------------------------------------
 * print_chip_information()
 *------------------------------------------------------------*/

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

/*--------------------------------------------------------------
 * red_task()
 *------------------------------------------------------------*/

static void red_task(void *pvParameter)
{
    /* Wait for task to be called via vTaskResume(). */
    vTaskSuspend(NULL);

    /* Loop forever. */
    for (;;)
    {
        ESP_LOGE(TAG, "Beginning red_task().");
        for (uint8_t i = 1; i <= RYG_TASK_SECONDS; i++)
        {
            /* Wait by busy waiting. This is typically a bad idea because it
             * prevents other tasks from running, so typically waiting by
             * *blocking* is a better idea. However, this is intended to
             * demonstrate task preemption, so it is required. */
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
            ESP_LOGE(TAG, "red_task(): %u second.", i);
        }
        ESP_LOGE(TAG, "Ending red_task().");
        led_strip.state = OFF;
        led_strip_update();

        /* Wait for task to be called via vTaskResume(). */
        vTaskSuspend(NULL);
    }

    /* It should never reach here. */
    vTaskDelete(NULL);
}

/*--------------------------------------------------------------
 * yellow_task()
 *------------------------------------------------------------*/

static void yellow_task(void *pvParameter)
{
    /* Wait for task to be called via vTaskResume(). */
    vTaskSuspend(NULL);

    /* Loop forever. */
    for (;;)
    {
        ESP_LOGW(TAG, "Beginning yellow_task().");
        for (uint8_t i = 1; i <= RYG_TASK_SECONDS; i++)
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
            ESP_LOGW(TAG, "yellow_task(): %u second.", i);
        }
        ESP_LOGW(TAG, "Ending yellow_task().");
        led_strip.state = OFF;
        led_strip_update();

        /* Wait for task to be called via vTaskResume(). */
        vTaskSuspend(NULL);
    }
    
    /* It should never reach here. */
    vTaskDelete(NULL);
}

/*--------------------------------------------------------------
 * green_task()
 *------------------------------------------------------------*/

static void green_task(void *pvParameter)
{
    /* Wait for task to be called via vTaskResume(). */
    vTaskSuspend(NULL);

    /* Loop forever. */
    for (;;)
    {
        ESP_LOGI(TAG, "Beginning green_task().");
        for (uint8_t i = 1; i <= RYG_TASK_SECONDS; i++)
        {
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
            ESP_LOGI(TAG, "green_task(): %u second.", i);
        }
        ESP_LOGI(TAG, "Ending green_task().");
        led_strip.state = OFF;
        led_strip_update();

        /* Wait for task to be called via vTaskResume(). */
        vTaskSuspend(NULL);
    }
    
    /* It should never reach here. */
    vTaskDelete(NULL);
}

/*==============================================================
 * LED strip.
 *============================================================*/

/*--------------------------------------------------------------
 * led_strip_configure()
 *------------------------------------------------------------*/

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

/*--------------------------------------------------------------
 * led_strip_update()
 *------------------------------------------------------------*/

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

/*==============================================================
 * UART.
 *============================================================*/

/*--------------------------------------------------------------
 * uart_configure()
 *------------------------------------------------------------*/

static void uart_configure(void)
{
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    /* We won't use a buffer for sending data. */
    uart_driver_install(UART_NUM_1, UART_RX_BUFFER_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

/*--------------------------------------------------------------
 * uart_rx_task()
 *------------------------------------------------------------*/

static void uart_rx_task(void *arg)
{
    /* Initialize variables. */
    static const char *UART_RX_TASK_TAG = "UART_RX_TASK";
    esp_log_level_set(UART_RX_TASK_TAG, ESP_LOG_INFO);
    uint8_t *data = (uint8_t *)malloc(UART_RX_BUFFER_SIZE + 1);

    /* Loop forever. */
    for (;;)
    {
        /* Check if there is received data. */
        if (is_debug_on)
        {
            ESP_LOGI(UART_RX_TASK_TAG, "Checking if there is received data.");
        }
        const int rx_bytes = uart_read_bytes(UART_NUM_1, data, UART_RX_BUFFER_SIZE, pdMS_TO_TICKS(100));
        if (rx_bytes > 0)
        {
            /* Idk what this does. It seems to set the end of what was read
             * to `0`, but why? */
            data[rx_bytes] = 0;

            if (is_debug_on)
            {
                ESP_LOGI(UART_RX_TASK_TAG, "Read %d bytes: '%s'.", rx_bytes, data);
                ESP_LOG_BUFFER_HEXDUMP(UART_RX_TASK_TAG, data, rx_bytes, ESP_LOG_INFO);
            }

            /* Convert `data` to a string. */
            char data_string[128];
            int i = 0;
            int index = 0;
            for (i = 0; i < rx_bytes; i++)
            {
                index += snprintf(&data_string[index], 128 - index, "%c", data[i]);
            }
            if (is_debug_on)
            {
                ESP_LOGI(UART_RX_TASK_TAG, "data_string = %s.", data_string);
            }

            /* Determine what was read and then do something. */
            if (strcmp(data_string, "leader_red_task") == 0)
            {
                vTaskResume(red_task_handle);
            }
            else if (strcmp(data_string, "leader_yellow_task") == 0)
            {
                vTaskResume(yellow_task_handle);
            }
            else if (strcmp(data_string, "leader_green_task") == 0)
            {
                vTaskResume(green_task_handle);
            }
            else if (strcmp(data_string, "follower_toggle_led") == 0)
            {
                follower_toggle_led();
            }
            else
            {
                ESP_LOGE(UART_RX_TASK_TAG, "Error: Did not understand command.");
            }
        }
    }

    /* It should never reach here. */
    free(data);
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
    uart_configure();
    zigbee_configure();

    xTaskCreate(
        &red_task,
        "red_task",
        TASK_STACK_DEPTH,
        NULL,
        RED_TASK_PRIORITY,
        &red_task_handle);
    xTaskCreate(
        &yellow_task,
        "yellow_task",
        TASK_STACK_DEPTH,
        NULL,
        YELLOW_TASK_PRIORITY,
        &yellow_task_handle);
    xTaskCreate(
        &green_task,
        "green_task",
        TASK_STACK_DEPTH,
        NULL,
        GREEN_TASK_PRIORITY,
        &green_task_handle);
    xTaskCreate(
        &uart_rx_task,
        "uart_rx_task",
        TASK_STACK_DEPTH,
        NULL,
        UART_RX_TASK_PRIORITY,
        NULL);

    /* Turn the LED off. Something (maybe the "Zigbee Green Power
     * enable" configuration setting) turns it bright green for an
     * unkown reason. This solution seems to work okay. The LED
     * briefly turns green, then turns off. */
    led_strip.state = OFF;
    led_strip_clear(led_strip.handle);
}
