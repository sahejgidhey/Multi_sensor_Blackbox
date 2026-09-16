#ifndef MPU6050_DRIVER_H
#define MPU6050_DRIVER_H

#include "driver/i2c_master.h"
#define MPU6050_ADDR 0X68

void mpu6050_init(i2c_master_bus_handle_t bus_handle , i2c_master_dev_handle_t* mpu6050_handle);
void mpu6050_get_data(i2c_master_dev_handle_t mpu6050_handle , float* return_data);

#endif 