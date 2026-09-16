#include "include/bmp280_driver.h"


// this is the calib dig used for calibrating raw temp and pressure data
uint16_t dig_T1;
int16_t  dig_T2, dig_T3;
uint16_t dig_P1;
int16_t  dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;

int32_t t_fine; // this is the fine temprature value

void bmp280_init(i2c_master_bus_handle_t bus_handle, i2c_master_dev_handle_t *bmp280_handle)
{

    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_7,
        .device_address = BMP280_ADDR,
        .scl_speed_hz = 100000 // 100Hz
    };

    i2c_master_bus_add_device(bus_handle , &dev_config , bmp280_handle);

    // this will wake up sensor
    uint8_t setup_data[2] = {0xF4, 0x93};
    i2c_master_transmit(*bmp280_handle, setup_data, 2, 1000); // this will transmit 0x23 bit to start temp reading 

}

void calibration_data(i2c_master_dev_handle_t bmp280_handle) 
{

    uint8_t addr = 0x88;
    uint8_t data[24];

    i2c_master_transmit_receive(bmp280_handle , &addr , 1 , data , 24 , 10);

    dig_T1 = (uint16_t)((data[1] << 8) | data[0]);
    dig_T2 = (int16_t)((data[3] << 8) | data[2]);
    dig_T3 = (int16_t)((data[5] << 8) | data[4]);
    
    dig_P1 = (uint16_t)((data[7] << 8) | data[6]);
    dig_P2 = (int16_t)((data[9] << 8) | data[8]);
    dig_P3 = (int16_t)((data[11] << 8) | data[10]);
    dig_P4 = (int16_t)((data[13] << 8) | data[12]);
    dig_P5 = (int16_t)((data[15] << 8) | data[14]);
    dig_P6 = (int16_t)((data[17] << 8) | data[16]);
    dig_P7 = (int16_t)((data[19] << 8) | data[18]);
    dig_P8 = (int16_t)((data[21] << 8) | data[20]);
    dig_P9 = (int16_t)((data[23] << 8) | data[22]);

}

int32_t temprature_calibrate(int32_t adc_T) 
{
    int32_t var1, var2, T;
    
    var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
    
    t_fine = var1 + var2; // Crucial: assigns value to global t_fine
    T = (t_fine * 5 + 128) >> 8;
    
    return T;
}

uint32_t pressure_calibrate(int32_t adc_P) 
{
    int64_t var1, var2, p;
    
    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)dig_P6;
    var2 = var2 + ((var1 * (int64_t)dig_P5) << 17);
    var2 = var2 + (((int64_t)dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)dig_P3) >> 8) + ((var1 * (int64_t)dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)dig_P1) >> 33;
    
    if (var1 == 0) 
    {
        return 0; // Avoid division by zero
    }
    
    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7) << 4);
    
    return (uint32_t)p;
}

uint32_t format_bit(uint8_t* bits , uint8_t index)
{

    return (uint32_t)((bits[index] << 12) | (bits[index+1] << 4) | (bits[index+2] >> 4));

}

void bmp280_get_data(i2c_master_dev_handle_t bmp280_handle , float* output_data)
{

    uint8_t addr = 0xF7;
    uint8_t data[6] = {0};

    i2c_master_transmit_receive(bmp280_handle , &addr , 1 , data , 6 , 10);

    uint32_t raw_pressure = format_bit(data , 0);
    uint32_t raw_temprature = format_bit(data , 3);

    calibration_data(bmp280_handle);
    output_data[0] = (float)pressure_calibrate(raw_pressure) / 256.0f / 100.0f; // this will convert data into hpa
    output_data[1] = (float)temprature_calibrate(raw_temprature) / 100.0f;

}
