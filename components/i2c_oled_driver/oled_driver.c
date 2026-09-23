#include "include/oled_driver.h"
#include "esp_log.h"
#include <string.h>

static i2c_master_dev_handle_t oled_handle_here;

void command_write(uint8_t* command , int command_buff_size)
{

    uint8_t write_buff[1+command_buff_size];
    write_buff[0] = 0x00;

    memcpy(&write_buff[1] , command , command_buff_size);

    i2c_master_transmit(oled_handle_here , write_buff , 1+command_buff_size , -1);

}

void oled_init_commands()
{

    uint8_t init_command_1 = 0xAE; // 1. Turn display off (Sleep mode)
    command_write(&init_command_1 , 1);

    uint8_t init_command_2[2] = {0xD5, 0x50}; // 2. Set Clock Divide Ratio / Oscillator Frequency
    command_write(init_command_2 , 2);
    
    uint8_t init_command_3[2] = {0xA8, 0x3F}; // 3. Set Multiplex Ratio (128x64 resolution configuration)
    command_write(init_command_3 , 2);
    
    uint8_t init_command_4[2] = {0xD3, 0x00}; // 4. Set Display Offset to 0 (no vertical shift)
    command_write(init_command_4 , 2);
    
    uint8_t init_command_5 = 0x40; // 5. Set Display Start Line to 0
    command_write(&init_command_5 , 1);
    
    uint8_t init_command_6[2] = {0x8D, 0x14}; // 6. Enable the Charge Pump (CRITICAL: Steps up 3.3V to OLED voltage)
    command_write(init_command_6 , 2);

    uint8_t init_command_7[2] = {0x20, 0x00}; // 7. Set Memory Addressing Mode to Horizontal Mode
    command_write(init_command_7 , 2);

    uint8_t init_command_8 = 0xA0; // 8. Set Segment Re-map (Horizontal Flip so content is right-side up)
    command_write(&init_command_8 , 1);
    
    uint8_t init_command_9 = 0xC0; // 9. Set COM Output Scan Direction (Vertical Flip)
    command_write(&init_command_9 , 1);
    
    uint8_t init_command_10[2] = {0xDA, 0x12}; // 10. Set COM Pins Hardware Configuration
    command_write(init_command_10 , 2);
    
    uint8_t init_command_11[2] = {0x81, 0xCF}; // 11. Set Contrast (Brightness level) to roughly 80%
    command_write(init_command_11 , 2);
    
    uint8_t init_command_12[2] = {0xD9, 0x22}; // 12. Set Pre-charge Period
    command_write(init_command_12 , 2);
    
    uint8_t init_command_13[2] = {0xDB, 0x40}; // 13. Set VCOMH Deselect Level
    command_write(init_command_13 , 2);

    uint8_t init_command_14 = 0xA4; // 14. Resume Display to follow RAM content
    command_write(&init_command_14 , 1);
    
    uint8_t init_command_15 = 0xA6; // 15. Set Normal Display Mode (Non-inverted)
    command_write(&init_command_15 , 1);

    uint8_t buffer_temp[1024];
    memset(buffer_temp , 0x00 , 1024);

    oled_push_buffer(buffer_temp);
    
    uint8_t init_command_16 = 0xAF; // 16. Turn Display ON (Wake up!)
    command_write(&init_command_16 , 1);

}

void oled_init(i2c_master_bus_handle_t bus_handle, i2c_master_dev_handle_t* oled_handle)
{

    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_7,
        .device_address = OLED_ADDR,
        .scl_speed_hz = 400 * 1000 // 400KHz
    };

    i2c_master_bus_add_device(bus_handle , &dev_config , oled_handle);

    oled_handle_here = *oled_handle;

    oled_init_commands();

}

void oled_push_buffer(uint8_t* buffer)
{

    static uint8_t tx_buffer[1025] = {0x40};

    // Reset internal pointers to top left corner of the grid bounds
    uint8_t commands_column[3] = {0x21,0x00,0x7f};
    command_write(commands_column , 3);

    uint8_t commands_rows[3] = {0x22,0x00,0x07};
    command_write(commands_rows , 3);

    memcpy(&tx_buffer[1] , buffer , 1024);

    i2c_master_transmit(oled_handle_here , tx_buffer , 1025 , -1);

}