# How to use this library
### 1. Include this library
Edit your `main/cmakelists.txt`.

```cmake
idf_component_register(
    SRCS "main.cpp"
    PRIV_REQUIRES tb600b_v2 # Add this line
    INCLUDE_DIRS "." "include"
)
```

### 2. Code
Example:
```cpp
extern "C" void app_main(void)
{
    tb600b_handle_t *h2s_sensor =
        tb600b_init(H2S_UART_PORT, H2S_TX_PIN, H2S_RX_PIN, BAUD_RATE, H2S_LOG_TAG, TB600B_SENSOR_TYPE_H2S);

    tb600b_handle_t *so2_sensor =
        tb600b_init(SO2_UART_PORT, SO2_TX_PIN, SO2_RX_PIN, BAUD_RATE, SO2_LOG_TAG, TB600B_SENSOR_TYPE_SO2);

    if (h2s_sensor == nullptr || so2_sensor == nullptr)
    {
        ESP_LOGE("MAIN", "One or both sensors failed to initialize.");
        return;
    }

    tb600b_set_passive_mode(h2s_sensor);
    tb600b_set_passive_mode(so2_sensor);

    vTaskDelay(pdMS_TO_TICKS(1000));

    while (1)
    {
        // Do measurement
        tb600b_measure_and_update(h2s_sensor);
        vTaskDelay(pdMS_TO_TICKS(100)); 
        tb600b_measure_and_update(so2_sensor);
        
        // Store the data
        float g_h2s_temperature = tb600b_get_temperature(h2s_sensor);
        float g_h2s_humidity = tb600b_get_humidity(h2s_sensor);
        float g_h2s_gas_ug = tb600b_get_gas_ug(h2s_sensor);
        
        vTaskDelay(pdMS_TO_TICKS(4500));
    }
}
```

### 4. Build flash and Monitor