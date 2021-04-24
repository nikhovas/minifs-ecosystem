#pragma once


// #ifndef KERNEL_MODULE
    // #include <stdint.h>
// #endif
#include <minifs/std-wrapper/stdint.h>


typedef struct __attribute__((packed)) {
    uint8_t data[32];
} block_data_t;


block_data_t get_block_data_by_id(uint8_t block_id, int *error);
void write_bock_data_by_id(uint8_t block_id, block_data_t *block_data, int *error);


uint8_t alloc_block(int *error);
void free_block(uint8_t block_id, int *error);