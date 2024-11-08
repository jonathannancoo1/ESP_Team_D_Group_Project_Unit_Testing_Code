
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


static const char *TAG = "main";


void config_I2C_for_RTC(){
//configs for pins
i2c_config_t pinconfig;



//Installing driver
i2c_driver_install(I2C_NUM_0,I2C_MODE_MASTER);
 //master mode
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
printf("here1");

//passing params to function
//i2c_param_config(I2C_NUM_0,&pinconfig);

    //seting pins
i2c_set_pin(I2C_NUM_0, 0, 2,GPIO_PULLUP_DISABLE, GPIO_PULLUP_DISABLE, I2C_MODE_MASTER);

}






void write_to_control_reg(){

i2c_cmd_handle_t handlr2=i2c_cmd_link_create();
i2c_master_start(handlr2);
i2c_master_write_byte(handlr2,0xD0<<1 | I2C_MASTER_WRITE, 0x01);
i2c_master_write_byte(handlr2, 0x0E, true);  // Register address 0x0E
i2c_master_write_byte(handlr2, 0x0E, true);  // Register address 0x0E
esp_err_t ret=i2c_master_cmd_begin(I2C_NUM_0,handlr2,1000 / portTICK_RATE_MS);

i2c_master_stop(handlr2);


 i2c_cmd_link_delete(handlr2);

    // Check for success
    if (ret == ESP_OK) {
        printf("Data written successfully to register 0x0E\n");
    } else if (ret == ESP_ERR_INVALID_ARG) {
        printf("Invalid arguments provided to I2C command\n");
    } else {
        printf("Error occurred during I2C write: %d\n", ret);
    }


}


void i2c_master_read_rtc_register(){

uint8_t data = 0;


    // Create a command handle
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();


    i2c_master_start(cmd);
    // Write the slave address (0xD0) with the write bit (<<1 to add write bit)
    i2c_master_write_byte(cmd, 0xD0 << 1 | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x0F, true);
    // Send a repeated start condition for reading
    i2c_master_start(cmd);

    // Send the slave address (0xD0) with the read bit (<<1 and OR with 1)

    i2c_master_write_byte(cmd, 0xD0 << 1 | I2C_MASTER_READ, true);


    // Read the data byte from the register
    i2c_master_read_byte(cmd, &data, I2C_MASTER_NACK);  // NACK to end read


     // Stop I2C communication
    //i2c_master_stop(cmd);



    // Execute the command and send it on the bus
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);


    // Delete the command handle
    i2c_cmd_link_delete(cmd);

    // Check for success and return the data
    if (ret == ESP_OK) {
        printf("Data read from register 0x0F: 0x%02X\n", data);
    } else if (ret == ESP_ERR_INVALID_ARG) {
        printf("Invalid arguments provided to I2C command\n");
    } else {
        printf("Error occurred during I2C read: %d\n", ret);
    }


    printf("Data read from register (hexadecimal): 0x%02X\n", data);

}





void app_main(){


printf("hello world");

config_I2C_for_RTC();
write_to_control_reg();
i2c_master_read_rtc_register();
i2c_driver_delete(I2C_NUM_0);
i2cdev_init();
struct time_struct time = {
        .year = 116, //since 1900 (2016 - 1900)
        .month  = 9,  // 0-based
        .day = 9,
        .hour = 13,
        .min  = 50,
        .sec  = 10
    };

i2c_config_t pinconfig;



//Installing driver
//i2c_driver_install(I2C_NUM_0,I2C_MODE_MASTER);
 //master mode
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
    dev.addr=DS3231_ADDR;
    dev.cfg=pinconfig;
    dev.port=I2C_NUM_0;

    ds3231_init_desc(&dev,0,GPIO_NUM_2,GPIO_NUM_0);

while (1)
    {
        int temp;

        vTaskDelay(pdMS_TO_TICKS(250));

    ///setting time
         if (ds3231_set_time(&dev, &time) == ESP_OK)
        {
            printf("TImeset\n");
            continue;
        }


        //Pulling Time

        if (ds3231_get_time(&dev, &time) != ESP_OK)
        {
            printf("Could not get time\n");
            continue;
        }



}
