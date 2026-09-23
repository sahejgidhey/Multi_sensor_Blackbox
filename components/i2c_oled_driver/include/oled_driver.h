#ifndef OLED_DRIVER_H
#define OLED_DRIVER_H

#include "driver/i2c_master.h"
#define OLED_ADDR 0X3C

void oled_init(i2c_master_bus_handle_t bus_handle , i2c_master_dev_handle_t* oled_handle);
void oled_push_buffer(uint8_t* buffer);

#endif