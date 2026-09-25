#include "app_config.h"

i2c_master_bus_handle_t bus_handle;
i2c_master_dev_handle_t mpu6050_handle;
i2c_master_dev_handle_t bmp280_handle;
i2c_master_dev_handle_t oled_handle;

QueueHandle_t sensor_data_queue = NULL; 
SemaphoreHandle_t bus_mutex = NULL;

void mpu6050_sensor_task(void *vParametes)
{

    float mpu6050_data[7];

    while(1)
    {
        
        mpu6050_get_data(mpu6050_handle , mpu6050_data);
        vTaskDelay(pdMS_TO_TICKS(50));

    }

}

void bmp280_sensor_task(void *vParameters)
{

    float bmp280_data[2];

    while(1)
    {

        bmp280_get_data(bmp280_handle , bmp280_data);
        vTaskDelay(pdMS_TO_TICKS(50));

    }

}

void oled_print_task(void *vParameters)
{

    uint8_t buffer[1024];

    while(1)
    {

        memset(buffer , 0x0 , 1024);
        mpu6050_page(buffer);
        oled_push_buffer(buffer);

        vTaskDelay(pdMS_TO_TICKS(100));

    }

}

void app_main(void)
{

    // this all function initilazes sensors and oled on i2c bus
    i2c_init(&bus_handle);
    mpu6050_init(bus_handle , &mpu6050_handle);
    bmp280_init(bus_handle , &bmp280_handle);
    oled_init(bus_handle , &oled_handle);

    xTaskCreatePinnedToCore(oled_print_task , "Oled Print Screen" , 3072 , NULL , 2 , NULL , 0);

}
