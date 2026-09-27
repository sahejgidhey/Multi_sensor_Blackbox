#include "app_config.h"

i2c_master_bus_handle_t bus_handle;
i2c_master_dev_handle_t mpu6050_handle;
i2c_master_dev_handle_t bmp280_handle;
i2c_master_dev_handle_t oled_handle;

QueueHandle_t sensor_data_queue = NULL;// this for task data transfer
QueueHandle_t sensor_request_queue = NULL;// this will contain the sensor id which need to send data
SemaphoreHandle_t bus_mutex = NULL; // this is for resource management 

// these are used for isr and debounce control
static QueueHandle_t button_event_queue = NULL;
uint64_t volatile last_intrrupt_time = 0;

// this is the isr handler used for to get button click 
static void IRAM_ATTR button_isr_handler(void *args)
{

    uint32_t pin_num = (uint32_t) args;
    uint64_t current_time = esp_timer_get_time();
    
    if((current_time - last_intrrupt_time) > debounce_time)
    {

        if(xQueueSendFromISR(button_event_queue , &pin_num , NULL) == pdTRUE)
        {

            last_intrrupt_time = current_time;

        }

    }

}

// this task runs and get mpu6050 data
void mpu6050_sensor_task(void *vParametes)
{

    // this contains the sensor data for mpu6050
    static float mpu6050_data[7];
    // this is used for sending and recieving data from queue
    sensor_data_t sensor_data;
    uint16_t sensor_id = 0; // this contains the sensor id of whos data the task want

    while(1)
    {
        
        if(xQueueReceive(sensor_request_queue , &sensor_id , portMAX_DELAY) == pdPASS) // this will check which sensor data is needed
        {


            if(sensor_id == 6050) // this check if the sensor id is correct and there is no data transmitted with it
            {

                // if data is recived then it will get the mutex
                if(xSemaphoreTake(bus_mutex,portMAX_DELAY) == pdTRUE)
                {

                    ESP_LOGI("MPU6050" , "Mutex taken");

                    mpu6050_get_data(mpu6050_handle , mpu6050_data); // this get the data from sensor
                    sensor_data.data = mpu6050_data;// this puts data in struct
                    sensor_data.sensor_id = 6050;// this the sensor id of the sensor which is transmitting data

                    // this sends data to queue 
                    if(xQueueSend(sensor_data_queue , (void*) &sensor_data , pdMS_TO_TICKS(20)) == pdTRUE)
                    {

                        ESP_LOGI("MPU6050" , "Sensor data sent");

                    }

                    // this releases the i2c mutex
                    if(xSemaphoreGive(bus_mutex) == pdTRUE)
                    {

                        ESP_LOGI("MPU6050" , "Mutex released");

                    }

                }


            }

        }

        vTaskDelay(pdMS_TO_TICKS(50));

    }    

}

void bmp280_sensor_task(void *vParameters)
{

    float bmp280_data[2]; // this containes the sensor data 
    sensor_data_t sensor_data; // this is the sensor data which is transmitted on the queue
    uint16_t sensor_id = 0; // this contains the sensor who's data need to be printed

    while(1)
    {

        if(xQueueReceive(sensor_request_queue , &sensor_id , portMAX_DELAY) == pdTRUE) // this will get the sensor id from the queue
        {
            
            if(sensor_id == 280) // this checks if the sensor id is correct 
            {

                if(xSemaphoreTake(bus_mutex , portMAX_DELAY) == pdTRUE) // this wait for the i2c line to be free
                {

                    ESP_LOGI("BMP280" , "Mutex taken");

                    bmp280_get_data(bmp280_handle , bmp280_data); // this get the sensor data
                    sensor_data.data = bmp280_data;// this put the sensor data in the sensor data struct for queue
                    sensor_data.sensor_id = 280;// this is the sensor id of the sensor sending data

                    if(xQueueSend(sensor_data_queue , (void*) &sensor_data , pdMS_TO_TICKS(20)) == pdTRUE) // this sends the data to sensor data queue
                    {

                        ESP_LOGI("BMP280" , "Sensor data sent");

                    }

                    if(xSemaphoreGive(bus_mutex) == pdTRUE) // this releses the mutex when the work is done
                    {

                        ESP_LOGI("BMP280" , "Mutex released");

                    }

                }

            }

        }

        vTaskDelay(pdMS_TO_TICKS(50));

    }

}

void oled_print_task(void *vParameters)
{

    static uint8_t buffer[1024];
    sensor_data_t sensor_data;

    while(1)
    {

        if(xQueueReceive(sensor_data_queue , &sensor_data , portMAX_DELAY) == pdTRUE) // this will get data from the sensor data queue
        {

            if(sensor_data.data == NULL) // this checks if the data is null
            {

                ESP_LOGW("OLED" , "Data is null");
                vTaskDelay(pdMS_TO_TICKS(100));
                continue;

            }

            if(xSemaphoreTake(bus_mutex , portMAX_DELAY) == pdTRUE) // this wait for the i2c mutex
            {

                memset(buffer , 0x0 , 1024);

                ESP_LOGI("OLED" , "Mutex released");

                if(sensor_data.sensor_id == 6050) // this checkn the sensor id of the sensor transmitting data
                {

                    ESP_LOGI("OLED" , "Sensor id->6050");
                    mpu6050_page(buffer , sensor_data.data);
                    oled_push_buffer(buffer);

                }
                else if(sensor_data.sensor_id == 280)
                {

                    ESP_LOGI("OLED" , "Sensor id->280");
                    bmp280_page(buffer , sensor_data.data);
                    oled_push_buffer(buffer);
                    
                }

                if(xSemaphoreGive(bus_mutex) == pdTRUE) // this releases i2c mutex after the work is done
                {

                    ESP_LOGI("OLED" , "Mutex released");

                }

            }
        
        }

        vTaskDelay(pdMS_TO_TICKS(100));

    }

}

void button_task(void *vParameters)
{

    uint32_t pin_num = 0;
    uint16_t sensor_id = 0; // this is the sensor id of sensor which needs to be active 
    int flag = 1; // this will check the when the button is clicked

    while(1)
    {

        if(xQueueReceive(button_event_queue , &pin_num , pdMS_TO_TICKS(20)) == pdTRUE) // this checks if the button is pressed and wait for only 20 milliseconds for queue
        {

            // this changes the sensor id when buttonn is pressed
            if(flag == 1)
            {

                sensor_id = 6050;
                flag++;

            }
            else if(flag == 2)
            {

                sensor_id = 280;
                flag = 1;

            }

        }

        // this sends the sensor id which data need to printed to queue
        if(xQueueSend(sensor_request_queue , &sensor_id , pdMS_TO_TICKS(20)) == pdTRUE)
        {

            ESP_LOGI("Button" , "Command sent");

        }

        vTaskDelay(pdMS_TO_TICKS(50));

    }

}

void app_main(void)
{

    button_event_queue = xQueueCreate(5 , sizeof(uint32_t)); // this contains button clicked data
    sensor_data_queue = xQueueCreate(5 , sizeof(sensor_data_t)); // this contains sensor data and id of sensor transmitting data 
    sensor_request_queue = xQueueCreate(5 , sizeof(uint16_t)); // this contains the sensor of the sensor which data is required
    bus_mutex = xSemaphoreCreateMutex();

    // this all function initilazes sensors and oled on i2c bus
    i2c_init(&bus_handle);
    mpu6050_init(bus_handle , &mpu6050_handle);
    bmp280_init(bus_handle , &bmp280_handle);
    oled_init(bus_handle , &oled_handle);
    button_config();

    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON,button_isr_handler,(void*) BUTTON);

    xTaskCreatePinnedToCore(oled_print_task , "Oled Print Screen" , 5102 , NULL , 2 , NULL , 0);
    xTaskCreatePinnedToCore(button_task , "Button control" , 2048 , NULL , 3 , NULL , 0);
    xTaskCreatePinnedToCore(mpu6050_sensor_task , "MPU Sensor" , 3072 , NULL , 1 , NULL , 1);
    xTaskCreatePinnedToCore(bmp280_sensor_task , "BMP280 Sensor" , 3072 , NULL , 1 , NULL , 1);

} 
