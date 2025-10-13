#ifndef UART_USER_CONFIG_H
#define UART_USER_CONFIG_H

#include "hal/uart_types.h"
#include "sdkconfig.h"

#define UART_BAUD_RATE 9600

#ifdef CONFIG_IDF_TARGET CONFIG_IDF_TARGET_ESP32
#define SO2_UART_PORT UART_NUM_1
#define SO2_UART_RX_PIN 16
#define SO2_UART_TX_PIN 17

#define H2S_UART_PORT UART_NUM_2
#define H2S_UART_RX_PIN 26
#define H2S_UART_TX_PIN 27
#endif

#ifdef CONFIG_IDF_TARGET CONFIG_IDF_TARGET_ESP32C3
#define SO2_UART_PORT UART_NUM_0
#define SO2_UART_RX_PIN 20
#define SO2_UART_TX_PIN 21

#define H2S_UART_PORT UART_NUM_1
#define H2S_UART_RX_PIN 9
#define H2S_UART_TX_PIN 10
#endif

#if !defined(CONFIG_IDF_TARGET_ESP32) && !defined(CONFIG_IDF_TARGET_ESP32C3)
#error "Unsupported IDF Target! Please configure for ESP32 or ESP32C3."
#endif

static const int RX_BUF_SIZE = 128;

#endif // UART_USER_CONFIG_H