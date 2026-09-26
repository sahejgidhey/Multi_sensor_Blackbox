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
    sensor_data_t sensor_data;

    while(1)
    {
        
        if(xQueueReceive(sensor_data_queue , &sensor_data , portMAX_DELAY) == pdPASS)
        {

            ESP_LOGI("MPU6050" , "Data recived");

            if(xSemaphoreTake(bus_mutex,portMAX_DELAY) == pdTRUE)
            {

                ESP_LOGI("MPU6050" , "Mutex taken");

                if(sensor_data.sensor_id == 6050)
                {

                    mpu6050_get_data(mpu6050_handle , mpu6050_data);
                    // ESP_LOGI("MPU6050" , "Data[0]->%.2f" , mpu6050_data[0]);
                    // ESP_LOGI("MPU6050" , "Data[1]->%.2f" , mpu6050_data[1]);
                    // ESP_LOGI("MPU6050" , "Data[2]->%.2f" , mpu6050_data[2]);
                    // ESP_LOGI("MPU6050" , "Data[3]->%.2f" , mpu6050_data[3]);
                    // ESP_LOGI("MPU6050" , "Data[4]->%.2f" , mpu6050_data[4]);
                    // ESP_LOGI("MPU6050" , "Data[5]->%.2f" , mpu6050_data[5]);
                    // ESP_LOGI("MPU6050" , "Data[6]->%.2f" , mpu6050_data[6]);
                    sensor_data.data = mpu6050_data;
                    sensor_data.sensor_id = 6050;

                }

                if(xQueueSend(sensor_data_queue , (void*) &sensor_data , pdMS_TO_TICKS(20)) == pdTRUE)
                {

                    ESP_LOGI("MPU6050" , "Sensor data sent");

                }

                xSemaphoreGive(bus_mutex);

            }

        }
        else
        {
            ESP_LOGI("MPU6050" , "Data not recived");
        }

        vTaskDelay(pdMS_TO_TICKS(50));

    }    

}

void bmp280_sensor_task(void *vParameters)
{

    float bmp280_data[2];
    sensor_data_t sensor_data;

    while(1)
    {

        bmp280_get_data(bmp280_handle , bmp280_data);
        vTaskDelay(pdMS_TO_TICKS(50));

    }

}

void oled_print_task(void *vParameters)
{

    static uint8_t buffer[1024];
    sensor_data_t sensor_data;

    while(1)
    {

        sensor_data.sensor_id = 6050;

        if(xQueueSend(sensor_data_queue , (void*) &sensor_data , pdMS_TO_TICKS(20)) == pdTRUE)
        {
 
            ESP_LOGI("OLED" , "Data send");

            //vTaskDelay(pdMS_TO_TICKS(50));

            if(xQueueReceive(sensor_data_queue , &sensor_data , portMAX_DELAY) == pdTRUE)
            {

                if(xSemaphoreTake(bus_mutex , portMAX_DELAY) == pdTRUE)
                {

                
                    memset(buffer , 0x0 , 1024);
                    mpu6050_page(buffer , sensor_data.data);
                    oled_push_buffer(buffer);

                    xSemaphoreGive(bus_mutex);

                }

            }

        }
        else
        {
            ESP_LOGI("OLED" , "Data not send");
        }

        vTaskDelay(pdMS_TO_TICKS(100));

    }

}

void app_main(void)
{

    sensor_data_queue = xQueueCreate(5 , sizeof(sensor_data_t));
    bus_mutex = xSemaphoreCreateMutex();

    // this all function initilazes sensors and oled on i2c bus
    i2c_init(&bus_handle);
    mpu6050_init(bus_handle , &mpu6050_handle);
    bmp280_init(bus_handle , &bmp280_handle);
    oled_init(bus_handle , &oled_handle);

    xTaskCreatePinnedToCore(oled_print_task , "Oled Print Screen" , 5102 , NULL , 2 , NULL , 0);
    xTaskCreatePinnedToCore(mpu6050_sensor_task , "MPU Sensor" , 3072 , NULL , 1 , NULL , 1);

}
