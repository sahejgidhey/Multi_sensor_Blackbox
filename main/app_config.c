#include "app_config.h"

void i2c_init(i2c_master_bus_handle_t *bus_handle)
{ 

    gpio_reset_pin(SDA_PIN);
    gpio_reset_pin(SCL_PIN);

    i2c_master_bus_config_t bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_0,
        .scl_io_num = SCL_PIN,
        .sda_io_num = SDA_PIN,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true
    };

    i2c_new_master_bus(&bus_config,bus_handle);

    ESP_LOGI("I2C" , "Initialized");

}