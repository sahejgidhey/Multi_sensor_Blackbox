#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "include/mpu6050_driver.h"
#include "include/bmp280_driver.h"
#include "include/oled_driver.h"
#include "include/oled_functions.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "ui_manager.h"

#define SDA_PIN GPIO_NUM_21
#define SCL_PIN GPIO_NUM_22
#define BUTTON GPIO_NUM_34 

typedef struct sensor_data_temp {
    uint16_t sensor_id;
    float *data;
}sensor_data_t;

void i2c_init(i2c_master_bus_handle_t *bus_handle);

#endif