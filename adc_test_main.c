
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



void app_main(){

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
ads111x_init_desc(&dev,ADS111X_ADDR_GND,I2C_NUM_0,GPIO_NUM_2,GPIO_NUM_0)==ESP_OK
printf("hello world")





}