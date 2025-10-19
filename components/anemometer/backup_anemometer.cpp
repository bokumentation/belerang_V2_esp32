#include <cstdint>
#include <freertos/FreeRTOS.h>
#include "freertos/task.h"
#include "anemometer.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_timer.h" 

#define GPIO_PULSE_PIN GPIO_NUM_32

static const char *pTAG_ANEMOMETER = "ANEMOMETER";
volatile unsigned int rpmcount = 0;

uint64_t timeold_us = 0;
const uint64_t TIMEMEASURE_US = 10000000ULL; 

float current_speed_kmph = 0.0;
float current_speed_mps = 0.0;

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    __atomic_add_fetch(&rpmcount, 1, __ATOMIC_RELAXED);
}

// --- Setup Function ---
void anemometer_init(void)
{
    // 1. Configure the GPIO pin
    gpio_config_t io_conf = {}; // Zero-initialize the struct in C++

    io_conf.intr_type = GPIO_INTR_POSEDGE;  // RISING edge interrupt
    io_conf.mode = GPIO_MODE_INPUT;         // Set as input
    io_conf.pin_bit_mask = (1ULL << GPIO_PULSE_PIN); // Only for the specified pin
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE; // No pull-down
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;      // Enable internal pull-up resistor

    gpio_config(&io_conf);

    // 2. Install the global GPIO ISR handler
    gpio_install_isr_service(0); 

    // 3. Hook the specific ISR function to the pin
    gpio_isr_handler_add(GPIO_PULSE_PIN, gpio_isr_handler, (void *)GPIO_PULSE_PIN);

    // 4. Initialize timing variable
    timeold_us = esp_timer_get_time();

    ESP_LOGI(pTAG_ANEMOMETER, "Anemometer initialized on GPIO %d.", GPIO_PULSE_PIN);
}

// --- Loop/Measurement Function ---
void anemometer_measure_and_update(void)
{
    // Use the highly accurate microsecond timer
    uint64_t current_time_us = esp_timer_get_time();

    // Check if the measurement interval has passed.
    if ((current_time_us - timeold_us) >= TIMEMEASURE_US)
    {
        // 1. Disable interrupt to safely read and reset rpmcount.
        gpio_intr_disable(GPIO_PULSE_PIN);

        // 2. Use a temporary variable and reset the counter and timer.
        unsigned int current_rpmcount = rpmcount;
        rpmcount = 0;
        timeold_us = current_time_us; // Reset time to current time

        // 3. Re-enable the interrupt.
        gpio_intr_enable(GPIO_PULSE_PIN); 

        // 4. Perform calculations outside of the protected section.
        const float time_interval_s = (float)TIMEMEASURE_US / 1000000.0f;
        
        float rotations_per_second = (float)current_rpmcount / time_interval_s;

        // Your conversion formula (example: for a specific anemometer model)
        float speed_m_per_s = (-0.0181 * (rotations_per_second * rotations_per_second)) + 
                              (1.3859 * rotations_per_second) + 
                              1.4055;

        // A simple conditional to handle low wind speeds.
        if (speed_m_per_s <= 1.5)
        {
            speed_m_per_s = 0.0;
        }

        float speed_km_per_h = speed_m_per_s * 3.6f;

        // Store the calculated values in the global variables
        current_speed_kmph = speed_km_per_h;
        current_speed_mps = speed_m_per_s;

        // Log the results.
        ESP_LOGI(pTAG_ANEMOMETER, "Rotations per second: %.2f", rotations_per_second);
        ESP_LOGI(pTAG_ANEMOMETER, "Speed (m/s): %.2f", speed_m_per_s);
        ESP_LOGI(pTAG_ANEMOMETER, "Speed (km/h): %.2f", speed_km_per_h);
    }
}

// --- Getter Functions ---
float getAnemometerSpeed_kmph(void) {
    return current_speed_kmph;
}

float getAnemometerSpeed_mps(void) {
    return current_speed_mps;
}