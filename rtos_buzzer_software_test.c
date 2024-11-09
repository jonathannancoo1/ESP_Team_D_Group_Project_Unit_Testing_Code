#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "esp_system.h"
#include "esp_err.h"

#include "esp8266/gpio_register.h"
#include "esp8266/pin_mux_register.h"

#include "driver/pwm.h"

// Main application function
void app_main(){
    // Initial message to indicate the test has started
    printf("Start_Test \n");

    // Get the start tick count for calculating execution time later
    TickType_t start_tick = xTaskGetTickCount();

    // PWM configuration parameters
    uint32_t PWM_LED_Period = 500;  // PWM period
    uint32_t duty_cycle = 250;      // Duty cycle for the PWM
    uint8_t channel = 1;            // PWM channel
    const uint32_t Pin_0 = 0;       // PWM output pin
    int c = 1;                      // Control variable for timing measurement

    // Initialize PWM with defined period, duty cycle, and pin
    esp_err_t ret = pwm_init(PWM_LED_Period, &duty_cycle, channel, &Pin_0);
    if (ret == ESP_OK) {
        printf("PWM initialized successfully\n");
    } else {
        printf("PWM initialization failed\n");
    }

    while (1) {
        // Start PWM output
        esp_err_t start = pwm_start();
        if (start == ESP_OK) {
            printf("PWM started successfully\n");
        } else {
            printf("Failed to start PWM\n");
        }

        // Execute timing measurement block only once (when c == 1)
        if (c == 1) {
            // Measure and print execution time since the start tick
            TickType_t end_tick = xTaskGetTickCount();
            TickType_t tick_difference = end_tick - start_tick;
            printf("Execution time: %d ticks\n", tick_difference);

            // Convert ticks to milliseconds
            int execution_time_ms = tick_difference * portTICK_PERIOD_MS;
            printf("Execution time: %d milliseconds\n", execution_time_ms);
        }

        // Run PWM for 1 second, then stop
        vTaskDelay(pdMS_TO_TICKS(1000));
        pwm_stop(0x0);  // Stop all PWM channels
        printf("PWM stopped for 1 second\n");

        if (c == 1) {
            // Measure and print cumulative execution time
            TickType_t end_tick2 = xTaskGetTickCount();
            TickType_t tick_difference2 = end_tick2 - start_tick;
            printf("Execution time: %d ticks\n", tick_difference2);

            // Convert ticks to milliseconds
            int execution_time_ms = tick_difference2 * portTICK_PERIOD_MS;
            printf("Execution time: %d milliseconds\n", execution_time_ms);
        }

        // Set control variable to avoid repeated execution of timing block
        c = 2;

        // Pause for 1 second before restarting PWM
        vTaskDelay(pdMS_TO_TICKS(1000));

        // Start PWM again
        esp_err_t start2 = pwm_start();
        if (start2 == ESP_OK) {
            printf("PWM restarted successfully\n");
        } else {
            printf("Failed to restart PWM\n");
        }

        // Run PWM for 2 seconds, then stop
        vTaskDelay(pdMS_TO_TICKS(2000));
        pwm_stop(0x0);
        printf("PWM stopped for 2 seconds\n");

        // Pause for 2 seconds before next loop iteration
        vTaskDelay(pdMS_TO_TICKS(2000));

        // Start PWM again
        esp_err_t start5 = pwm_start();
        if (start5 == ESP_OK) {
            printf("PWM started successfully for the final time\n");
        } else {
            printf("Failed to start PWM\n");
        }

        // Run PWM for 5 seconds, then stop
        vTaskDelay(pdMS_TO_TICKS(5000));
        pwm_stop(0x0);
        printf("PWM stopped for 5 seconds\n");

        // Pause for 5 seconds before repeating the loop
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
