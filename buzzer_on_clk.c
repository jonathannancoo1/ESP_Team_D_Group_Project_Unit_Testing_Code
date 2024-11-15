
//Buzzer on Clock LED used As placeholder

//  Same as Calibrate w two extra lines
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
#include "ds3231.h"
#include <i2cdev.h>
#include <string.h>
#include "esp_err.h"

void app_main(){



//Installing i2c Device Driver
i2cdev_init();

struct tm time_to_calibrate={

    .tm_year=124,
    .tm_mon=11,
    .tm_mday=9,
    .tm_hour=16,
    .tm_min=36,
    .tm_sec=0
};


//Setting Up Struct with Pin Configurations
i2c_config_t pinconfig;


//I2C master
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



//Device specific driver
    i2c_dev_t dev;
    memset(&dev, 0, sizeof(i2c_dev_t));
    dev.addr=DS3231_ADDR;
    dev.cfg=pinconfig;
    dev.port=I2C_NUM_0;

    esp_err_t installer = ds3231_init_desc(&dev,0,GPIO_NUM_2,GPIO_NUM_0);
    if (installer==ESP_OK){

        printf("Set up Properly \n");
    }
    else{

        printf("Error");


    }


    esp_err_t calibrate;
    calibrate=ds3231_set_time(&dev,&time_to_calibrate);

    if(calibrate==ESP_OK){

        printf("Time has been set\n");

            // tm_year is the number of years since 1900, so 124 + 1900 = 2024
    // tm_mon is the month, with January as 0, so 11 is December

    printf("Year: %d\n", time_to_calibrate.tm_year + 1900);  // Add 1900 to tm_year
    printf("Month: %d\n", time_to_calibrate.tm_mon + 1);     // Add 1 to tm_mon (0-based)
    printf("Day: %d\n", time_to_calibrate.tm_mday);
    printf("Hour: %d\n", time_to_calibrate.tm_hour);
    printf("Minute: %d\n", time_to_calibrate.tm_min);
    printf("Second: %d\n", time_to_calibrate.tm_sec);
    }


    else{

        printf("Error TS again \n");
    }
ds3231_set_squarewave_freq(&dev,DS3231_SQWAVE_1HZ);

ds3231_enable_squarewave(&dev);
    while(1){

        printf("in loop \n");

        vTaskDelay(5000/portTICK_PERIOD_MS);


    }
}
