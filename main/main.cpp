#include <freertos/FreeRTOS.h>
#include <cstdio>
#include <stdio.h>
#include "esp_log.h"
#include "freertos/task.h"

// USER INCLUDE
#include "hal/uart_types.h"
#include "portmacro.h"
#include "tb600b_so2.h"
#include "uart_user_config.h"

static const char *SO2_UART_TAG = "[SO2]";
static const char *H2S_UART_TAG = "[H2S]";

// Global variable to store value
float g_so2_current_temperature = 0.0;
float g_so2_current_humidity = 0.0;
float g_so2_current_gas_ug = 0.0;

// Global variable to store value
float g_h2s_current_temperature = 0.0;
float g_h2s_current_humidity = 0.0;
float g_h2s_current_gas_ug = 0.0;

const char *TAG2 = "ini tag";

extern "C" void app_main(void) {
  init_uart((uart_port_t)SO2_UART_PORT, SO2_UART_TX_PIN, SO2_UART_RX_PIN, UART_BAUD_RATE, SO2_UART_TAG);
  init_uart((uart_port_t)H2S_UART_PORT, H2S_UART_TX_PIN, H2S_UART_RX_PIN, UART_BAUD_RATE, H2S_UART_TAG);

  vTaskDelay(pdMS_TO_TICKS(1000));

  while (1) {
    tb600b_get_combined_data((uart_port_t)SO2_UART_PORT, CMD_GET_COMBINED_DATA, sizeof(CMD_GET_COMBINED_DATA), SO2_UART_TAG,
                             &g_so2_current_temperature, &g_so2_current_humidity, &g_so2_current_gas_ug);
    ESP_LOGI(SO2_UART_TAG, "Temperature: %.2f %%", g_so2_current_temperature);
    ESP_LOGI(SO2_UART_TAG, "Humidity: %.2f %%", g_so2_current_humidity);
    ESP_LOGI(SO2_UART_TAG, "Gas: %.2f %%", g_so2_current_gas_ug);
    vTaskDelay(pdMS_TO_TICKS(1000));

    tb600b_get_combined_data((uart_port_t)H2S_UART_PORT, CMD_GET_COMBINED_DATA, sizeof(CMD_GET_COMBINED_DATA), H2S_UART_TAG,
                             &g_h2s_current_temperature, &g_h2s_current_humidity, &g_h2s_current_gas_ug);
    ESP_LOGI(H2S_UART_TAG, "Temperature: %.2f %%", g_h2s_current_temperature);
    ESP_LOGI(H2S_UART_TAG, "Humidity: %.2f %%", g_h2s_current_humidity);
    ESP_LOGI(H2S_UART_TAG, "Gas: %.2f %%", g_h2s_current_gas_ug);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
