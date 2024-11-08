/* SPIFFS filesystem example.
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_timer.h"

// Timer callback function - called when the timer expires
void my_timer_callback(void* arg) {
    printf("Timer expired!\n");
    // Perform actions here (keep them short for minimal delay)
}

// Function to initialize a periodic microsecond timer
void init_microsecond_timer() {
    // Define the timer configuration
    const esp_timer_create_args_t timer_args = {
        .callback = &my_timer_callback,           // Set the callback function
        .arg = NULL,                              // Argument to pass to the callback
        .dispatch_method = ESP_TIMER_TASK,        // Run the callback in the timer task
        .name = "microsecond_timer"               // Name of the timer
    };

    // Create the timer
    esp_timer_handle_t timer_handle;
    esp_err_t err = esp_timer_create(&timer_args, &timer_handle);
    if (err != ESP_OK) {
        printf("Failed to create timer: %s\n", esp_err_to_name(err));
        return;
    }

    // Start the timer with a period of 100000 microseconds (100 ms)
    err = esp_timer_start_periodic(timer_handle, 100000);
    if (err != ESP_OK) {
        printf("Failed to start timer: %s\n", esp_err_to_name(err));
        return;
    }

    printf("Microsecond timer started\n");
}

// Log tag used for debugging output
static const char *TAG = "example";

// Main application entry point
void app_main(void)
{
    // Measure start time for SPIFFS initialization
    int32_t start_time = esp_timer_get_time(); // Start time in microseconds

    ESP_LOGI(TAG, "Initializing SPIFFS");

    // SPIFFS configuration settings
    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",                   // Mount path for SPIFFS
      .partition_label = NULL,                  // Label for SPIFFS partition
      .max_files = 5,                           // Maximum number of open files
      .format_if_mount_failed = true            // Format SPIFFS if mount fails
    };

    // Initialize and mount SPIFFS filesystem
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    // Check if SPIFFS mounted successfully
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }

        // Measure end time and calculate duration of failed initialization
        int32_t end_time = esp_timer_get_time();   // End time in microseconds
        int32_t duration = end_time - start_time;  // Calculate elapsed time
        printf("Execution time: %d microseconds\n", duration);
        return;
    }

    // Get SPIFFS partition information: total and used space
    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }

    /* Uncomment the following block to write and rename a file in SPIFFS

    ESP_LOGI(TAG, "Opening file");
    FILE* f = fopen("/spiffs/hello.txt", "w");   // Open file for writing
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }
    fprintf(f, "NVS WORK!\n");                   // Write to the file
    fclose(f);                                   // Close the file
    ESP_LOGI(TAG, "File written");

    // Check if the destination file exists before renaming
    struct stat st;
    if (stat("/spiffs/foo.txt", &st) == 0) {
        unlink("/spiffs/foo.txt");               // Delete existing file if it exists
    }

    // Rename the original file
    ESP_LOGI(TAG, "Renaming file");
    if (rename("/spiffs/hello.txt", "/spiffs/foo.txt") != 0) {
        ESP_LOGE(TAG, "Rename failed");
        return;
    }
    */

    // Measure start time for file reading
    start_time = esp_timer_get_time();
    ESP_LOGI(TAG, "Reading file");
    
    // Open renamed file for reading
    FILE* f = fopen("/spiffs/foo.txt", "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return;
    }
    
    // Read a line from the file
    char line[64];
    fgets(line, sizeof(line), f);
    fclose(f);

    // Strip newline character if present
    char* pos = strchr(line, '\n');
    if (pos) {
        *pos = '\0';
    }
    ESP_LOGI(TAG, "Read from file: '%s'", line);

    // Measure end time and calculate duration of file reading
    int32_t end_time2 = esp_timer_get_time();
    int32_t duration2 = end_time2 - start_time;  // Calculate elapsed time
    printf("Execution time: %d microseconds\n", duration2);

    // Unmount SPIFFS filesystem
    esp_vfs_spiffs_unregister(NULL);
    ESP_LOGI(TAG, "SPIFFS unmounted");
}
