#pragma once

#include <minifs/std-wrapper/stdint.h>


typedef struct __attribute__((packed)) {
    int file_size: 15;
    int is_folder: 1;
    uint8_t block_ids[6];
} i_node_data_t;


typedef struct {
    uint8_t number_of_blocks;
    uint8_t last_block_size;
} i_node_block_info_t;


i_node_block_info_t get_i_node_block_info(i_node_data_t *i_node_data);


// disk operations
i_node_data_t i_node_id__get_data(uint8_t i_node_id, int *error);
void i_node__save(uint8_t i_node_id, i_node_data_t *i_node_data, int *error);


// registration operations
uint8_t i_node__alloc(int *error);
void i_node_id__free(uint8_t i_node_id, int *error);


uint8_t i_node__get_block_count(i_node_data_t *i_node_data);
void i_node__add_blocks_to_end(i_node_data_t *i_node_data, uint8_t new_blocks, int* error);
void i_node__remove_blocks_from_end(i_node_data_t *i_node_data, uint8_t remove_block_count, int *error);
void i_node__set_block_count(i_node_data_t *i_node_data, uint8_t new_block_size, int * error);