//This code is used to calibrate the real time clock i.e. set the time
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

    struct tm time_read;
    esp_err_t read;
    read=ds3231_get_time(&dev,&time_read);

    if(read==ESP_OK){

        printf("Time has been read\n");

            // tm_year is the number of years since 1900, so 124 + 1900 = 2024
    // tm_mon is the month, with January as 0, so 11 is December

    printf("Year: %d\n", time_read.tm_year + 1900);  // Add 1900 to tm_year
    printf("Month: %d\n", time_read.tm_mon + 1);     // Add 1 to tm_mon (0-based)
    printf("Day: %d\n", time_read.tm_mday);
    printf("Hour: %d\n", time_read.tm_hour);
    printf("Minute: %d\n", time_read.tm_min);
    printf("Second: %d\n", time_read.tm_sec);
    }


    else{

        printf("Error try again \n");
    }


    while(1){


        vTaskDelay(60000/portTICK_PERIOD_MS);

        read=ds3231_get_time(&dev,&time_read);

        
    if(read==ESP_OK){

        printf("Time has been read\n");

            // tm_year is the number of years since 1900, so 124 + 1900 = 2024
    // tm_mon is the month, with January as 0, so 11 is December

    printf("Year: %d\n", time_read.tm_year + 1900);  // Add 1900 to tm_year
    printf("Month: %d\n", time_read.tm_mon + 1);     // Add 1 to tm_mon (0-based)
    printf("Day: %d\n", time_read.tm_mday);
    printf("Hour: %d\n", time_read.tm_hour);
    printf("Minute: %d\n", time_read.tm_min);
    printf("Second: %d\n", time_read.tm_sec);
    }


    else{

        printf("Error try again \n");
    }



    }
}
