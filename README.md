# ESP32C3 + TB600B S02
### Used pin:
- 20 RX
- 21 TX
- 9 RX
- 10 TX

### Powerline
- 3.3v
- GND


How to use the library:
```cpp
float g_so2_current_temperature = 0.0;
float g_so2_current_humidity = 0.0;
float g_so2_current_gas_ug = 0.0;

float g_h2s_current_temperature = 0.0;
float g_h2s_current_humidity = 0.0;
float g_h2s_current_gas_ug = 0.0;

// Cara panggil di main()

extern "C" void app_main(){
    // Panggil fungsi
    tb600b_get_combined_data(
      SO2_UART_PORT, 
      CMD_GET_COMBINED_DATA, 
      sizeof(CMD_GET_COMBINED_DATA), 
      SO2_UART_TAG,
      &g_so2_current_temperature,
      &g_so2_current_humidity,
      &g_so2_current_gas_ug
    );

    printf()
}
```




### Defined PIN
```cpp
// UART0 for S02
#define SO2_UART_PORT UART_NUM_0
#define SO2_UART_RX_PIN 20
#define SO2_UART_TX_PIN 21

// UART1 for H2S
#define H2S_UART_PORT UART_NUM_1
#define H2S_UART_RX_PIN 9
#define H2S_UART_TX_PIN 10
```

### Toolchain
- ESP-IDF-V5.5

### ChangelogS:
- 20250811
    - Added new UART port for H2S and S02. Now can read 'stimulesly / bersamaan'.
- 20250808
    - Added get Sensor data. But some the variable still unused.
- 20250807
    - Added UART commands: Led Query, LED ON, LED OFF.

