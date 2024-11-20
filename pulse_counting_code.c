#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_sleep.h"
#include "driver/gpio.h"
#include "driver/hw_timer.h"
#include "driver/uart.h"
#include "driver/i2c.h"
#include "freertos/semphr.h"
#include "freertos/FreeRTOSConfig.h"
#include "ads111x.h"
#include <i2cdev.h>
#include <string.h>


#define SAMPLE_RATE_HZ 64   // Data rate for the ADS111X, in samples per second
#define SAMPLE_DURATION_SEC 5 // Duration in seconds (1 second)
#define SAMPLE_COUNT (SAMPLE_RATE_HZ * SAMPLE_DURATION_SEC)-3 // Total samples to collect (128 for 1 second)
int8_t binary_buffer[SAMPLE_COUNT];


// Buffer to store ADC values
int16_t adc_buffer[SAMPLE_COUNT];

// Tag for logging (useful for debugging)
static const char *TAG = "ADC_Sample";

void app_main()
{
    i2cdev_init();

    i2c_config_t pinconfig;


    pinconfig.mode=I2C_MODE_MASTER;

    //disabling pullups
    pinconfig.sda_pullup_en=GPIO_PULLUP_DISABLE;
    pinconfig.scl_pullup_en=GPIO_PULLUP_DISABLE;


    //Setting SCL as IO 0
    pinconfig.scl_io_num=GPIO_NUM_0;

    //Setting SDA as IO2
    pinconfig.sda_io_num=GPIO_NUM_2;

    //wait for 1000 tics
    pinconfig.clk_stretch_tick = 1000;

    i2c_dev_t dev;
memset(&dev, 0, sizeof(i2c_dev_t));

dev.port=I2C_NUM_0 ;
dev.cfg=pinconfig;
dev.addr=ADS111X_ADDR_GND;
ads111x_init_desc(&dev,ADS111X_ADDR_GND,I2C_NUM_0,GPIO_NUM_2,GPIO_NUM_0);
ads111x_set_gain(&dev,ADS111X_GAIN_4V096);
ads111x_set_input_mux(&dev,ADS111X_MUX_0_GND);
ads111x_set_mode(&dev,ADS111X_MODE_CONTINUOUS);
ads111x_set_data_rate(&dev,ADS111X_DATA_RATE_128);

    unsigned long elapsed_time = 0;

   int sample_idx = 0;
    int16_t adc_value = 0;
  
    // Dynamically allocate buffer for storing ADC values
    int16_t *adc_buffer = (int16_t *)malloc(SAMPLE_COUNT * sizeof(int16_t));
    if (adc_buffer == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for ADC buffer");
        return;
    }
    unsigned long start_time= xTaskGetTickCount();

    while (elapsed_time < pdMS_TO_TICKS(SAMPLE_DURATION_SEC * 1000)) { // Run for 1 second
        // Get ADC value
        ads111x_get_value(&dev, &adc_value);

        // Store ADC value in buffer
        adc_buffer[sample_idx] = adc_value;

        // Increment sample index
        sample_idx++;

        // Log the sample
        ESP_LOGI(TAG, "1");


        // Update elapsed time
        elapsed_time = (xTaskGetTickCount() - start_time);

//        ESP_LOGI(TAG, "Time passed: %lu", elapsed_time);

    }
// Function to calculate frequency from the binary buffer
float calculate_frequency(int8_t *binary_buffer, int sample_count, int sample_rate_hz) {
    int transition_count = 0;
    float total_period = 0.0;
    
    // Iterate through the buffer and count the transitions
    for (int i = 1; i < sample_count; i++) {
        if (binary_buffer[i] != binary_buffer[i - 1]) { // A transition occurs
            transition_count++;
            // Calculate the period in terms of samples
            total_period += (i - (i - 1)); // Each transition is 1 sample apart for this simple case
        }
    }

    if (transition_count == 0) {
        return 0.0; // No transitions, so frequency cannot be calculated
    }

    // Calculate the average period (in samples)
    float average_period_samples = total_period / transition_count;
    
    // Convert average period from samples to seconds
    float period_seconds = average_period_samples / sample_rate_hz;
    
    // Frequency is the inverse of the period
    return 1.0 / period_seconds;
}

    for (int i = 0; i < sample_idx; i++) {
        if (adc_buffer[i] >= 13946) {
            binary_buffer[i] = 1; // Store 1 if value is above or equal to threshold
        } else {
            binary_buffer[i] = 0; // Store 0 if value is below threshold
        }
    }


    ESP_LOGI(TAG, "Sampling complete. %d samples collected.", sample_idx);

    // Print collected data
    ESP_LOGI(TAG, "ADC Values:");
    for (int i = 0; i < sample_idx; i++) {
        ESP_LOGI(TAG, "Sample %d: %d", i + 1, binary_buffer[i]); // Print each sample value
    }
    float frequency = calculate_frequency(&binary_buffer, 151, 30);
    // Free the dynamically allocated buffer
    free(adc_buffer);
    ESP_LOGI(TAG, "Calculated frequency: %.2f Hz", frequency);



}
