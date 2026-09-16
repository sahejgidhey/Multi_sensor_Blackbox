#include "include/mpu6050_driver.h"
#include "esp_log.h"

#define TAG "MPU6050"

void mpu6050_init(i2c_master_bus_handle_t bus_handle , i2c_master_dev_handle_t* mpu6050_handle)
{

    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_7,
        .device_address = MPU6050_ADDR,
        .scl_speed_hz = 400000 // 400hz for mpu 6050
    };

    esp_err_t err = i2c_master_bus_add_device(bus_handle , &dev_config , mpu6050_handle);

    if(err == ESP_OK)
    {

        ESP_LOGI(TAG , "Device configed");

    }
    else
    {

        ESP_LOGW(TAG , "Device not configed");

    }

}

uint16_t format_bits(uint8_t bit_1 , uint8_t bit_2)
{
    
    return (bit_1 << 8) | bit_2;

}

void raw_data_convert(uint8_t *data , float *return_data)
{

    uint16_t format_data[7]; // because this is 16 bit data 

    uint8_t j = 0; // this is used for indexing returning array
    for(int i = 0 ; i < 14 ; i+=2)
    {

        format_data[j] = format_bits(data[i] , data[i+1]);
        j++;

    }

    // this will convert raw accelerometer data to human readable data
    for(int i = 0 ; i < 3 ; i++)
    {

        return_data[i] = (float)format_data[i] / 16384.0f;

    }

    return_data[3] = ((float)format_data[3] / 340.0f) + 36.53f; // this is the temp data

    //this convert raw gyro data to human readable data
    for(int i = 3 ; i < 7 ; i++)
    {

        return_data[i] = (float)format_data[i]/131.0f;

    }

}

void mpu6050_get_data(i2c_master_dev_handle_t mpu6050_handle , float* return_data)
{

    uint8_t addr = 0x3B; // this is the address of register 
    uint8_t data[14] = {0}; // this will contain the recived data from mpu6050

    if(i2c_master_transmit_receive(mpu6050_handle , &addr , 1 , data , 14 , 10) == ESP_OK)
    {

        ESP_LOGI(TAG , "Data recived sucessfully");

    }

    raw_data_convert(data,return_data);

}
