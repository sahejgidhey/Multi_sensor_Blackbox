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

void button_config()
{

    gpio_config_t pin_config = {
        .pin_bit_mask = (1ULL << BUTTON),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE
    };

    gpio_config(&pin_config);

}