/*##############################################################
 * FILE INFO
 *############################################################*/

/* UART asynchronous example, that uses separate RX and TX tasks

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied. */

/*##############################################################
 * INCLUDES
 *############################################################*/

/*==============================================================
 * Standard.
 *============================================================*/

#include <stdlib.h>
#include <string.h>

/*==============================================================
 * ESP.
 *============================================================*/

#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "esp_system.h"
#include "led_strip.h"

/*==============================================================
 * FreeRTOS.
 *============================================================*/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/*##############################################################
 * DEFINES
 *############################################################*/

#define TXD_PIN (GPIO_NUM_4)
#define RXD_PIN (GPIO_NUM_17)
#define LED_STRIP_GPIO (GPIO_NUM_8)
#define TAG "main"

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

static const int RX_BUF_SIZE = 1024;

/*##############################################################
 * GLOBAL VARIABLES
 *############################################################*/

static led_strip_t led_strip;
static TaskHandle_t led_strip_green_task_handle = NULL;
static TaskHandle_t led_strip_red_task_handle = NULL;
static TaskHandle_t led_strip_yellow_task_handle = NULL;

/*##############################################################
 * FUNCTION PROTOTYPES
 *############################################################*/

static void led_strip_configure(void);

/*##############################################################
 * FUNCTIONS
 *############################################################*/

/*==============================================================
 * General.
 *============================================================*/

void init(void)
{
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    // We won't use a buffer for sending data.
    uart_driver_install(UART_NUM_1, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    led_strip_configure();
}

/*==============================================================
 * UART.
 *============================================================*/

int sendData(const char *logName, const char *data)
{
    const int len = strlen(data);
    const int txBytes = uart_write_bytes(UART_NUM_1, data, len);
    ESP_LOGI(logName, "Wrote %d bytes.", txBytes);
    return txBytes;
}

static void tx_task(void *arg)
{
    static const char *TX_TASK_TAG = "TX_TASK";
    esp_log_level_set(TX_TASK_TAG, ESP_LOG_INFO);
    while (1)
    {
        sendData(TX_TASK_TAG, "Hello world.\n");
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

static void rx_task(void *arg)
{
    static const char *RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    uint8_t *data = (uint8_t *)malloc(RX_BUF_SIZE + 1);
    while (1)
    {
        const int rxBytes = uart_read_bytes(UART_NUM_1, data, RX_BUF_SIZE, pdMS_TO_TICKS(1000));
        if (rxBytes > 0)
        {
            data[rxBytes] = 0;
            ESP_LOGI(RX_TASK_TAG, "Read %d bytes: '%s'.", rxBytes, data);
            ESP_LOG_BUFFER_HEXDUMP(RX_TASK_TAG, data, rxBytes, ESP_LOG_INFO);

            if (0)
            {
                char *message = "Read something: ";
                const int len = strlen(message);
                uart_write_bytes(UART_NUM_1, message, len);
            }
            
            switch (*data)
            {
            case 0:
            case 30:
            case 48:
                vTaskResume(led_strip_green_task_handle);
                break;
            case 1:
            case 31:
            case 49:
                vTaskResume(led_strip_yellow_task_handle);
                break;
            case 2:
            case 32:
            case 50:
                vTaskResume(led_strip_red_task_handle);
                break;
            }
        }
    }
    free(data);
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
            TickType_t ticks_to_wait = pdMS_TO_TICKS(1000);
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
            TickType_t ticks_to_wait = pdMS_TO_TICKS(1000);
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
            TickType_t ticks_to_wait = pdMS_TO_TICKS(1000);
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

/*##############################################################
 * MAIN
 *############################################################*/

void app_main(void)
{
    init();
    xTaskCreate(
        rx_task,
        "uart_rx_task",
        1024 * 2,
        NULL,
        configMAX_PRIORITIES - 1,
        NULL);
    xTaskCreate(
        tx_task,
        "uart_tx_task",
        1024 * 2,
        NULL,
        configMAX_PRIORITIES - 2,
        NULL);
    xTaskCreate(
        &led_strip_red_task,
        "led_strip_red_task",
        1024 * 2,
        NULL,
        configMAX_PRIORITIES - 3,
        &led_strip_red_task_handle);
    xTaskCreate(
        &led_strip_yellow_task,
        "led_strip_yellow_task",
        1024 * 2,
        NULL,
        configMAX_PRIORITIES - 4,
        &led_strip_yellow_task_handle);
    xTaskCreate(
        &led_strip_green_task,
        "led_strip_green_task",
        1024 * 2,
        NULL,
        configMAX_PRIORITIES - 5,
        &led_strip_green_task_handle);
}
