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
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// User Include
#include "anemometer.h"
#include "board_pins.h"
#include "data_format.h"
#include "tb600b_v2.h"

extern "C" void app_main(void)
{
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
        anemometer_measure_and_update(wind_sensor); // Updates every 10s non-blocking
        tb600b_measure_and_update(h2s_sensor);
        vTaskDelay(pdMS_TO_TICKS(100)); // Small delay between UART reads
        tb600b_measure_and_update(so2_sensor);

        // 2. AGGREGATE AND PRINT ALL DATA ONCE
        ESP_LOGI("MAIN", "--- Aggregating Cycle Data ---");

        // This function gathers the latest data from all handles and prints ONE
        // JSON line
        print_all_data_json(wind_sensor, h2s_sensor, so2_sensor);

        // 3. WAIT FOR THE NEXT MEASUREMENT CYCLE
        // Total delay is now 500ms + 4500ms = 5 seconds between SO2 read and H2S
        // read
        vTaskDelay(pdMS_TO_TICKS(4500));
    }
}