#include <stdio.h>
#include "esp_serial_slave_link/essl.h"
#include "driver/uart.h"

/* Task to read data from the UART. */
static void uart_task(void *pvParameter)
{
    /* Method 1. */
    if (0)
    {
        // printf("uart_task().\n");
        uint8_t data;
        int result = uart_read_bytes(UART_NUM_0, &data, 1, pdMS_TO_TICKS(1000));
        printf("uart_read_bytes(0) = %d.\n", result);
        if (result > 0)
        {
            printf("data = %u.\n", data);
        }
        // vTaskDelay(pdMS_TO_TICKS(1000));
    }

    /* Method 2. Based on ESP documentation. */
    for (;;)
    {
        const uart_port_t uart_num = UART_NUM_0;
        uint8_t data[128];
        int length = 0;
        ESP_ERROR_CHECK(uart_get_buffered_data_len(uart_num, (size_t*)&length));
        if (length > 0)
        {
            uart_read_bytes(uart_num, data, length, pdMS_TO_TICKS(1000));
            printf("data = ");
            for (int i = 0; i < length; i++)
            {
                printf("%d", data[i]);
            }
            printf(".\n");
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void app_main(void)
{
    /* Initialize. */
    if (0)
    {
        // essl_handle_t essl_handle;
        // essl_init(&essl_handle, 1000);
    }
    else if (1)
    {
        uart_config_t uart_config = {
            .baud_rate = 115200,
            .data_bits = UART_DATA_8_BITS,
            .parity = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};

        ESP_ERROR_CHECK(uart_param_config(UART_NUM_0, &uart_config));
        ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0, 2048, 2048, 0, NULL, 0));
        ESP_ERROR_CHECK(uart_param_config(UART_NUM_1, &uart_config));
        ESP_ERROR_CHECK(uart_driver_install(UART_NUM_1, 2048, 2048, 0, NULL, 0));

        /* Start the UART task. */
        xTaskCreate(
            &uart_task,
            "uart_task",
            2048,
            NULL,
            10,
            NULL);
    }

    for (;;)
    {
        printf("Waiting...\n");
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
