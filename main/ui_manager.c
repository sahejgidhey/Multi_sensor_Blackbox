#include "ui_manager.h"
#include <stdint.h>
#include <string.h>


void mpu6050_page(uint8_t* buffer)
{

    uint8_t buffer_temp[1024];
    memset(buffer_temp , 0x0 , 1024); 

    char name[] = "MPU6050";
    text_print(buffer_temp , name ,  sizeof(name) , 0 , 5 , 2);
    buffer_merge(buffer , buffer_temp , 1024);

    char cords[] = "Acc";
    text_print(buffer_temp , cords , sizeof(cords) , 1 , 2 , 5);
    buffer_merge(buffer , buffer_temp , 1024);

}