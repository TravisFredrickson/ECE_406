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

// #include <inttypes.h>
#include <math.h>
#include <stdio.h>

/*==============================================================
 * ESP.
 *============================================================*/

#include "driver/gpio.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_log.h"
#include "esp_system.h"
#include "led_strip.h"
#include "sdkconfig.h"

/*==============================================================
 * FreeRTOS.
 *============================================================*/

// #include "freertos/FreeRTOS.h"
// #include "freertos/queue.h"
// #include "freertos/task.h"

/*==============================================================
 * User.
 *============================================================*/

#include "button.h"

/*##############################################################
 * DEFINES
 *############################################################*/

#define MAIN_DELAY_PERIOD_MS 1000
#define DEBOUNCE_DELAY_PERIOD_MS 100

/*==============================================================
 * LED.
 *============================================================*/

#define LED_STRIP_GPIO 8

/*==============================================================
 * GPIO.
 *============================================================*/

/* Let's say, GPIO_OUTPUT_IO_0=18, GPIO_OUTPUT_IO_1=19
 * In binary representation,
 * 1ULL<<GPIO_OUTPUT_IO_0 is equal to 0000000000000000000001000000000000000000 and
 * 1ULL<<GPIO_OUTPUT_IO_1 is equal to 0000000000000000000010000000000000000000 and
 * GPIO_OUTPUT_PIN_SEL    is equal to 0000000000000000000011000000000000000000. */
#define GPIO_INPUT_IO_0 4
#define GPIO_INPUT_PIN_SEL ((1ULL << GPIO_INPUT_IO_0))

#define ESP_INTR_FLAG_DEFAULT 0

/*##############################################################
 * TYPEDEFS
 *############################################################*/

/* This could be in a separate file for better OOP. */
typedef struct
{
    led_strip_handle_t handle;
    enum
    {
        OFF,
        RED,
        GREEN,
        BLUE,
        LED_STATE_COUNT
    } state;
} led_strip_t;

/*##############################################################
 * CONSTANTS
 *############################################################*/

static const char *TAG = "ESP32-C6";

/*##############################################################
 * GLOBAL VARIABLES
 *############################################################*/

static led_strip_t s_led_strip;
static QueueHandle_t gpio_event_queue = NULL;
static int count = 0;

/*##############################################################
 * FUNCTION PROTOTYPES
 *############################################################*/

static void print_chip_information(void);
static void led_strip_configure(void);
static void led_strip_set(void);
static void gpio_configure(void);
static void gpio_isr_handler(void *);
static void gpio_task_example(void *);

/*##############################################################
 * FUNCTIONS
 *############################################################*/

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
 * LED.
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

static void led_strip_set(void)
{
    /* Set the LED based on its state. */
    if (s_led_strip.state == OFF)
    {
        ESP_LOGI(TAG, "Turning the LED \"OFF\"!");
        /* Set all LED off to clear all pixels. */
        led_strip_clear(s_led_strip.handle);
    }
    else if (s_led_strip.state == RED)
    {
        ESP_LOGI(TAG, "Turning the LED \"RED\"!");
        /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for
         * each color. */
        led_strip_set_pixel(s_led_strip.handle, 0, 16, 0, 0);
        /* Refresh the strip to send data. */
        led_strip_refresh(s_led_strip.handle);
    }
    else if (s_led_strip.state == GREEN)
    {
        ESP_LOGI(TAG, "Turning the LED \"GREEN\"!");
        led_strip_set_pixel(s_led_strip.handle, 0, 0, 16, 0);
        led_strip_refresh(s_led_strip.handle);
    }
    else if (s_led_strip.state == BLUE)
    {
        ESP_LOGI(TAG, "Turning the LED \"BLUE\"!");
        led_strip_set_pixel(s_led_strip.handle, 0, 0, 0, 16);
        led_strip_refresh(s_led_strip.handle);
    }
    else
    {
        ESP_LOGE(TAG, "Invalid LED state.\n");
    }
}

/*==============================================================
 * GPIO.
 *============================================================*/

static void gpio_configure(void)
{
    /* Zero-initialize the config structure. */
    gpio_config_t io_config = {};
    /* Interrupt on a falling edge. */
    io_config.intr_type = GPIO_INTR_NEGEDGE;
    /* Bit mask of the pins. */
    io_config.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    /* Set as input mode. */
    io_config.mode = GPIO_MODE_INPUT;
    /* Enable pull-up mode. */
    io_config.pull_up_en = 1;
    /* Configure. */
    gpio_config(&io_config);
    /* Print GPIO configuration. */
    // gpio_dump_io_configuration(stdout, ((int)pow(2, GPIO_PIN_COUNT) - 1));
    /* Create a queue to handle GPIO event from ISR. */
    gpio_event_queue = xQueueCreate(10, sizeof(uint32_t));
    /* Start GPIO task. */
    xTaskCreate(gpio_task_example, "gpio_task_example", 2048, NULL, 10, NULL);
    /* Install GPIO ISR service. */
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    /* Hook ISR handler for specific GPIO pin. */
    gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void *)GPIO_INPUT_IO_0);
}

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(gpio_event_queue, &gpio_num, NULL);
}

static void gpio_task_example(void *arg)
{
    uint32_t gpio_num;
    while (1)
    {
        /* Transfer from queue to buffer. */
        if (xQueueReceive(gpio_event_queue, &gpio_num, portMAX_DELAY))
        {
            printf("GPIO[%" PRIu32 "]: Interrupt; Value = %d.\n", gpio_num, gpio_get_level(gpio_num));
        }
        /* If input has been held down. */
        // if()
        {
            count++;
            ESP_LOGI(TAG, "count = %d.", count);
            s_led_strip.state++;
            s_led_strip.state %= LED_STATE_COUNT;
            led_strip_set();
        }
    }
}

/*##############################################################
 * MAIN
 *############################################################*/

void app_main(void)
{
    print_chip_information();
    led_strip_configure();
    // gpio_configure();

    button_event_t button_event;
    // QueueHandle_t button_events = button_init(PIN_BIT(4));
    QueueHandle_t button_events = pulled_button_init(PIN_BIT(4), GPIO_PULLUP_ONLY);

    /* Main loop. */
    while (1)
    {
        if (xQueueReceive(button_events, &button_event, MAIN_DELAY_PERIOD_MS / portTICK_PERIOD_MS))
        {
            if ((button_event.pin == 5) && (button_event.event == BUTTON_DOWN))
            {
                count++;
                ESP_LOGI(TAG, "count = %d.", count);
                s_led_strip.state++;
                s_led_strip.state %= LED_STATE_COUNT;
                led_strip_set();
            }
        }
        // vTaskDelay(MAIN_DELAY_PERIOD_MS / portTICK_PERIOD_MS);
    }
}
