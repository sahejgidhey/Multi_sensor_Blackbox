#ifndef OLED_FUNCTIONS_H
#define OLED_FUNCTIONS_H 
#include <stdint.h>

#define OLED_H_RES 128
#define OLED_V_RES 64

void buffer_merge(uint8_t *buffer_main , uint8_t *buffer_fn , int size);
void text_print(uint8_t *buffer_temp, char text[] , int text_size , uint8_t page, uint8_t front_space, uint8_t top_space);

#endif