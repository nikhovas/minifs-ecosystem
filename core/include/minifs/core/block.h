#pragma once


// #ifndef KERNEL_MODULE
    // #include <stdint.h>
// #endif
#include <minifs/std-wrapper/stdint.h>
#include <minifs/filesystem_context.h>


typedef struct __attribute__((packed)) {
    uint8_t data[32];
} block_data_t;


block_data_t get_block_data_by_id(struct minifs_core__filesystem_context * ctx, uint8_t block_id, int *error);
void write_bock_data_by_id(struct minifs_core__filesystem_context * ctx, uint8_t block_id, block_data_t *block_data, int *error);


uint8_t alloc_block(struct minifs_core__filesystem_context * ctx, int *error);
void free_block(struct minifs_core__filesystem_context * ctx, uint8_t block_id, int *error);