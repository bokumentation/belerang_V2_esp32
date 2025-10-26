/*
    ESP32 GAS DETECTOR: SO2 (Belerang) + H2S + ANEMOMETER
    Supported Target: ESP32, ESP32C3, ESP32S3

    Last Revision: 251019
    Using Microsoft's Style Guide

    Update:
    - clang-format add new style

    To do:
    - Add arduino-esp32 as ESP-IDF Components. We need this for Heltec LORA library
    - Add Global Variable and Pass the Reference. So can pass it in LORA.
    - Implement SPI SD CARD (SPI -> SD CARD Module)
    - Implement Battery level indicator (I2C -> INA Sensor)
    - Implement MQTT Client (SPI -> W5500)
    - Implement Serial Monitor for parsing data on WEB APP (UART 0)
*/

// System Include
#include "esp_log.h"
#include <cstdio>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// User Include
#include "anemometer.h"
#include "board_pins.h"
#include "data_format.h"
#include "freertos/projdefs.h"
#include "mcu_temp_handler.h"
#include "portmacro.h"
#include "tb600b_v2.h"

float g_anemometer_kmp = 0;
float g_h2s_temperature = 0;
float g_h2s_humidity = 0;
float g_h2s_gas_ug = 0;

float g_so2_temperature = 0;
float g_so2_humidity = 0;
float g_so2_gas_ug = 0;

extern "C" void app_main(void)
{
    mcu_temp_init();
    vTaskDelay(pdMS_TO_TICKS(1000));
    anemometer_handle_t *wind_sensor = anemometer_init(ANEMOMETER_ADC_PIN);
    if (wind_sensor == nullptr) {
        ESP_LOGE("MAIN", "Anemometer Failed");
        return;
    }

    tb600b_handle_t *h2s_sensor =
        tb600b_init(H2S_UART_PORT, H2S_TX_PIN, H2S_RX_PIN, BAUD_RATE, H2S_LOG_TAG, TB600B_SENSOR_TYPE_H2S);

    tb600b_handle_t *so2_sensor =
        tb600b_init(SO2_UART_PORT, SO2_TX_PIN, SO2_RX_PIN, BAUD_RATE, SO2_LOG_TAG, TB600B_SENSOR_TYPE_SO2);

    if (h2s_sensor == nullptr || so2_sensor == nullptr) {
        ESP_LOGE("MAIN", "One or both sensors failed to initialize.");
        return;
    }

    tb600b_set_passive_mode(h2s_sensor);
    tb600b_set_passive_mode(so2_sensor);

    vTaskDelay(pdMS_TO_TICKS(1000));

    while (1) {
        // 1. EXECUTE ALL MEASUREMENTS FIRST
        mcu_temp_read();
        float mcu_temp = mcu_get_temperature();

        anemometer_measure_and_update(wind_sensor); // Updates every 10s non-blocking
        tb600b_measure_and_update(h2s_sensor);
        vTaskDelay(pdMS_TO_TICKS(100)); // Small delay between UART reads
        tb600b_measure_and_update(so2_sensor);

        // Store the data
        g_anemometer_kmp = anemometer_get_speed_kmph(wind_sensor);

        g_h2s_temperature = tb600b_get_temperature(h2s_sensor);
        g_h2s_humidity = tb600b_get_humidity(h2s_sensor);
        g_h2s_gas_ug = tb600b_get_gas_ug(h2s_sensor);

        g_so2_temperature = tb600b_get_temperature(so2_sensor);
        g_so2_humidity = tb600b_get_humidity(so2_sensor);
        g_so2_gas_ug = tb600b_get_gas_ug(so2_sensor);

        // 2. AGGREGATE AND PRINT ALL DATA ONCE
        printf("MCU INFO: ");
        printf(" mcu_temp: %.2f", mcu_temp);
        printf(" |\n");

        printf("ANEMOMETER: ");
        printf(" wind_speed: %.2f", g_anemometer_kmp);
        printf(" |\n");

        printf("TB600C-H2S: ");
        printf(" temp: %.2f", g_h2s_temperature);
        printf(" | humid: %.2f", g_h2s_humidity);
        printf(" | h2s: %.2f", g_h2s_gas_ug);
        printf(" |\n");

        printf("TB600B-SO2: ");
        printf(" temp: %.2f", g_so2_temperature);
        printf(" | humid: %.2f", g_so2_humidity);
        printf(" | so2: %.2f", g_so2_gas_ug);
        printf(" |\n");

        ESP_LOGI("MAIN", "--- Aggregating Cycle Data ---");
        vTaskDelay(pdMS_TO_TICKS(3000)); // Delay 3 seconds
    }
}