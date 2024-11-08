/* Esptouch example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "tcpip_adapter.h"
#include "esp_smartconfig.h"
#include "smartconfig_ack.h"

/* The examples use smartconfig type that you can set via project configuration menu.

   If you'd rather not, just change the below entries to enum with
   the config you want - ie #define EXAMPLE_ESP_SMARTCOFNIG_TYPE SC_TYPE_ESPTOUCH
*/
#define EXAMPLE_ESP_SMARTCOFNIG_TYPE      SC_TYPE_ESPTOUCH

/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
static const int CONNECTED_BIT = BIT0;      // Bit flag indicating WiFi connection
static const int ESPTOUCH_DONE_BIT = BIT1;  // Bit flag indicating smartconfig process completion
static const char* TAG = "smartconfig_example";  // Logging tag

// Task to handle smartconfig
static void smartconfig_example_task(void* parm);

// Event handler for WiFi and smartconfig events
static void event_handler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        // Create smartconfig task when WiFi station starts
        xTaskCreate(smartconfig_example_task, "smartconfig_example_task", 4096, NULL, 3, NULL);
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        // Attempt to reconnect on disconnection
        esp_wifi_connect();
        xEventGroupClearBits(s_wifi_event_group, CONNECTED_BIT);
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        // Set connection bit when IP address is obtained
        xEventGroupSetBits(s_wifi_event_group, CONNECTED_BIT);
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_SCAN_DONE) {
        ESP_LOGI(TAG, "Scan done");  // Log scan completion
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_FOUND_CHANNEL) {
        ESP_LOGI(TAG, "Found channel");  // Log found channel
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD) {
        ESP_LOGI(TAG, "Got SSID and password");

        // Extract SSID and password from event data
        smartconfig_event_got_ssid_pswd_t* evt = (smartconfig_event_got_ssid_pswd_t*)event_data;
        wifi_config_t wifi_config;
        uint8_t ssid[33] = { 0 };
        uint8_t password[65] = { 0 };
        uint8_t rvd_data[33] = { 0 };

        bzero(&wifi_config, sizeof(wifi_config_t));  // Clear WiFi config structure
        memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));  // Copy SSID
        memcpy(wifi_config.sta.password, evt->password, sizeof(wifi_config.sta.password));  // Copy password
        wifi_config.sta.bssid_set = evt->bssid_set;

        // Copy BSSID if provided
        if (wifi_config.sta.bssid_set == true) {
            memcpy(wifi_config.sta.bssid, evt->bssid, sizeof(wifi_config.sta.bssid));
        }

        memcpy(ssid, evt->ssid, sizeof(evt->ssid));  // Copy SSID for logging
        memcpy(password, evt->password, sizeof(evt->password));  // Copy password for logging
        ESP_LOGI(TAG, "SSID:%s", ssid);
        ESP_LOGI(TAG, "PASSWORD:%s", password);

        // Log additional data if using ESPTouch V2
        if (evt->type == SC_TYPE_ESPTOUCH_V2) {
            ESP_ERROR_CHECK( esp_smartconfig_get_rvd_data(rvd_data, sizeof(rvd_data)) );
            ESP_LOGI(TAG, "RVD_DATA:%s", rvd_data);
        }

        // Disconnect WiFi and apply new configuration
        ESP_ERROR_CHECK(esp_wifi_disconnect());
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
        ESP_ERROR_CHECK(esp_wifi_connect());
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_SEND_ACK_DONE) {
        // Set smartconfig done bit when ACK is sent
        xEventGroupSetBits(s_wifi_event_group, ESPTOUCH_DONE_BIT);
    }
}

// Initialize WiFi and start smartconfig
static void initialise_wifi(void)
{
    tcpip_adapter_init();  // Initialize TCP/IP stack
    s_wifi_event_group = xEventGroupCreate();  // Create event group for WiFi events

    ESP_ERROR_CHECK(esp_event_loop_create_default());  // Create default event loop

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();  // Default WiFi initialization config

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));  // Initialize WiFi with config
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));  // Disable WiFi power-saving
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));  // Register WiFi event handler
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));  // Register IP event handler
    ESP_ERROR_CHECK(esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));  // Register smartconfig event handler

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));  // Set WiFi mode to STA (Station)
    ESP_ERROR_CHECK(esp_wifi_start());  // Start WiFi
}

// Task for handling smartconfig
static void smartconfig_example_task(void* parm)
{
    EventBits_t uxBits;
    ESP_ERROR_CHECK(esp_smartconfig_set_type(EXAMPLE_ESP_SMARTCOFNIG_TYPE));  // Set smartconfig type
    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();  // Default smartconfig config
    ESP_ERROR_CHECK(esp_smartconfig_start(&cfg));  // Start smartconfig

    while (1) {
        // Wait for connection or smartconfig completion
        uxBits = xEventGroupWaitBits(s_wifi_event_group, CONNECTED_BIT | ESPTOUCH_DONE_BIT, true, false, portMAX_DELAY);

        if (uxBits & CONNECTED_BIT) {
            ESP_LOGI(TAG, "WiFi Connected to ap");  // Log connection to access point
        }

        if (uxBits & ESPTOUCH_DONE_BIT) {
            ESP_LOGI(TAG, "smartconfig over");  // Log smartconfig completion
            esp_smartconfig_stop();  // Stop smartconfig
            vTaskDelete(NULL);  // Delete task
        }
    }
}

// Main application entry point
void app_main()
{
    ESP_ERROR_CHECK(nvs_flash_init());  // Initialize NVS
    initialise_wifi();  // Call WiFi initialization function
}
