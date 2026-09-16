#ifndef BMP280_DRIVER_H
#define BMP280_DRIVER_H

#include "driver/i2c_master.h"
#define BMP280_ADDR 0X76

void bmp280_init(i2c_master_bus_handle_t bus_handle , i2c_master_dev_handle_t* bmp280_handle);
void bmp280_get_data(i2c_master_dev_handle_t bmp280_handle , float* output_data);

#endif