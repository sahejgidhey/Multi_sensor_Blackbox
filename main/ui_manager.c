#include "ui_manager.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>


void mpu6050_page(uint8_t* buffer , float* data)
{

    if(buffer == NULL || data == NULL)
    {
        return;
    }

    uint8_t buffer_temp[1024];
    memset(buffer_temp , 0x0 , 1024); 

    char name[] = "MPU6050";
    text_print(buffer_temp , name ,  sizeof(name) , 0 , 36 , 3);
    buffer_merge(buffer , buffer_temp , 1024);

    text_print(buffer_temp , "ACC" , sizeof("ACC") , 2 , 5 , 0);
    text_print(buffer_temp , "GYRO" , sizeof("GYRO") , 2 , 47 , 0);
    text_print(buffer_temp , "TEMP" , sizeof("TEMP") , 2 , 92 , 0);
    buffer_merge(buffer , buffer_temp , 1024);

    text_print(buffer_temp , "X" , sizeof("X") , 3 , 5 , 5); // acc
    char num1[10];
    snprintf(num1 , sizeof(num1) , "%.2f" , data[0]);
    text_print(buffer_temp , num1 , sizeof("0000") , 3 , 9 , 5);

    text_print(buffer_temp , "X" , sizeof("X") , 3 , 47 , 5); // gyro
    char num2[10];
    snprintf(num2 , sizeof(num2) , "%.f" , data[4]);
    text_print(buffer_temp , num2 , sizeof("0000") , 3 , 51 , 5);

    char num3[10];
    snprintf(num3 , sizeof(num3) , "%.1f" , data[3]);
    text_print(buffer_temp , num3 , sizeof("0000") , 3 , 93 , 5);

    buffer_merge(buffer , buffer_temp , 1024);

    text_print(buffer_temp , "Y" , sizeof("Y") , 5 , 5 , 1); // acc
    char num4[10];
    snprintf(num4 , sizeof(num4) , "%.2f" , data[1]);
    text_print(buffer_temp , num4 , sizeof("0000") , 5 , 9 , 1);

    text_print(buffer_temp , "Y" , sizeof("Y") , 5 , 47 , 1); // gyro
    char num5[10];
    snprintf(num5 , sizeof(num5) , "%.f" , data[5]);
    text_print(buffer_temp , num5 , sizeof("0000") , 5 , 51 , 1);

    buffer_merge(buffer , buffer_temp , 1024);
    
    text_print(buffer_temp , "Z" , sizeof("Z") , 6 , 5 , 5); // acc
    char num6[10];
    snprintf(num6 , sizeof(num6) , "%.2f" , data[2]);
    text_print(buffer_temp , num6 , sizeof("0000") , 6 , 13 , 5);

    text_print(buffer_temp , "Z" , sizeof("Z") , 6 , 47 , 5); // gyro
    char num7[10];
    snprintf(num7 , sizeof(num7) , "%.f" , data[6]);
    text_print(buffer_temp , num7 , sizeof("0000") , 6 , 55 , 5);

    buffer_merge(buffer , buffer_temp , 1024);

}

void bmp280_page(uint8_t* buffer , float* data)
{

    uint8_t buffer_temp[1024];
    memset(buffer_temp , 0x0 , 1024);

    text_print(buffer_temp , "BMP280" , sizeof("BMP280") , 0 , 40 , 3);
    buffer_merge(buffer , buffer_temp , 1024);

    text_print(buffer_temp , "Pressure" , sizeof("Pressure") , 2 , 3 , 3);
    char num[4];
    snprintf(num , sizeof(num), "%.f" , data[0]);
    text_print(buffer_temp , num , sizeof(num) , 2 , 72 , 3);
    buffer_merge(buffer , buffer_temp , 1024);

    text_print(buffer_temp , "Temp" , sizeof("Temp") , 4 , 4 , 4);
    char num1[10];
    snprintf(num1 , sizeof(num1), "%.2f" , data[1]);
    text_print(buffer_temp , num1 , sizeof(num1) , 4 , 45 , 4);
    buffer_merge(buffer , buffer_temp , 1024);

}