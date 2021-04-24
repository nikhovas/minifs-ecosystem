#pragma once


#ifdef KERNEL_MODULE
    #include <linux/types.h>
#else
    #include <stdint.h>
#endif


uint8_t get_bit(uint8_t val, uint8_t bit_num);
uint8_t set_bit(uint8_t val, uint8_t bit_num);


char* trim_path(char *path);