/*
    ESP32 GAS DETECTOR: SO2 (Belerang) + H2S + ANEMOMETER
    Supported Target: ESP32, ESP32C3, ESP32S3
    Last Revision: 251026

    To do:
    - Implement INA219 Battery level indicator (I2C)
    - Implement RTC Clock DS3231 (I2C)
    - Implement SPI MICRO SDCARD MODULE (SPI)
    - Implement W5500 (SPI) for:
        - MQTT Client
        - Restful Server by using SPIFF partition (Opsional)
    - Implement Serial Monitor for parsing data on USB DESKTOP APP (USB JTAG)

    Check list:
    - TB600C-H2S (UART) Done
    - TB600B-SO2 (UART) Done
    - ANEMOMETER (ADC)  Done
*/

// System Include
#include "esp_log.h"
#include <cstdio>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// User Include
#include "anemometer.h"
#include "board_pins.h"
#include "freertos/projdefs.h"
#include "mcu_temp_handler.h"
#include "tb600b_v2.h"

// #include "data_format.h"
// #include "portmacro.h"

float g_anemometer_kmp = 0;  // Variabel for anemometer
float g_h2s_gas_ug = 0;      // Variabel h2s gas
float g_h2s_temperature = 0; // Variabel for h2s temp
float g_h2s_humidity = 0;    // Variabel h2s humid
float g_so2_gas_ug = 0;      // Variabel so2 gas
float g_so2_temperature = 0; // Variabel so2 temp
float g_so2_humidity = 0;    // Variabel so2 humid

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
        printf("ANEMOMETER: ");
        printf(" wind_speed: %.2f", g_anemometer_kmp);
        printf(" |\n");

        printf("%s: ", H2S_LOG_TAG);
        printf(" temp: %.2f", g_h2s_temperature);
        printf(" | humid: %.2f", g_h2s_humidity);
        printf(" | h2s: %.2f", g_h2s_gas_ug);
        printf(" |\n");

        printf("%s: ", SO2_LOG_TAG);
        printf(" temp: %.2f", g_so2_temperature);
        printf(" | humid: %.2f", g_so2_humidity);
        printf(" | so2: %.2f", g_so2_gas_ug);
        printf(" |\n");

        ESP_LOGI("MAIN", "--- Aggregating Cycle Data ---");
        vTaskDelay(pdMS_TO_TICKS(3000)); // Delay 3 seconds
    }
}
