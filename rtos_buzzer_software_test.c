//This is a unit test to flash an LED then Sound a Buzzer
//Places a PWM signal on GP0 of the ESP01

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


void pwm_control_task(void *pvParameters){

    printf("Start_Test \n");

    uint32_t PWM_LED_Period= 500;
    uint32_t duty_cycle=250;
    uint8_t channel=1;
    
    const uint32_t Pin_0=0;


    esp_err_t ret=pwm_init(PWM_LED_Period,&duty_cycle,channel,&Pin_0);
    if(ret==ESP_OK){
        printf("Success\n");
    }
    else{
        printf("Fail \n");
    }


    esp_err_t start=pwm_start();
    if(start==ESP_OK){
        printf("Success\n");
    }
    else{
        printf("Fail \n");
    }



            // Run PWM for 10 seconds
        vTaskDelay(pdMS_TO_TICKS(10000)); // Delay for 10 seconds

        // Stop PWM
        pwm_stop(0x0); // Pass 0x0 to stop all PWM channels
        printf("PWM stop for 50 seconds\n");

    }


void idle_task_test(void *pvParameters) {
    while (1) {
        printf("Idle task running, waiting for 50 seconds\n");
        vTaskDelay(pdMS_TO_TICKS(50000)); // Idle for 50 seconds
    }
}



void app_main() {
    //printf("Starting PWM Control Task\n");

    // Create the PWM control task
    xTaskCreate(pwm_control_task, "pwm_control_task", 2048, NULL, 1, NULL);
    xTaskCreate(idle_task_test, "idle_task_test", 2048, NULL, 1, NULL);

}

