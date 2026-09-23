idf_component_register(SRCS "oled_driver.c"  "oled_functions.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_driver_i2c)
