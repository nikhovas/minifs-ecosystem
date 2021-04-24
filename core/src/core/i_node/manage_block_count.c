#include "minifs/core/i_node.h"
#include "minifs/core/block.h"
#include "minifs/errors.h"


uint8_t i_node__get_block_count(i_node_data_t *i_node_data) {
    uint8_t block_count = i_node_data->file_size / 32;
    if (i_node_data->file_size % 32 != 0) {
        ++block_count;
    }
    return block_count;
}


void i_node__add_blocks_to_end(i_node_data_t *i_node_data, uint8_t new_blocks, int* error) {
    *error = NO_ERROR;

    const uint8_t block_count = i_node__get_block_count(i_node_data);
    for (uint8_t i = block_count; i < block_count + new_blocks; ++i) {
        i_node_data->block_ids[i] = alloc_block(error);
        if (*error != NO_ERROR) {
            for (int16_t j = i; j >= block_count; --j) {
                free_block(i_node_data->block_ids[j], error);
                if (*error != NO_ERROR) {
                    return;
                }

                i_node_data->block_ids[j] = 0;
            }
            return;
        }
    }
}


void i_node__remove_blocks_from_end(i_node_data_t *i_node_data, uint8_t remove_block_count, int *error) {
    uint8_t block_number = i_node_data->file_size / 32;
    for (uint8_t i = 0; i < remove_block_count; ++i) {
        uint8_t block_id = i_node_data->block_ids[block_number - 1 - i];
        i_node_data->block_ids[block_number - 1 - i] = 0;
        free_block(block_id, error);
        if (*error != NO_ERROR) {
            return;
        }
    }
}


void i_node__set_block_count(i_node_data_t *i_node_data, uint8_t new_block_size, int * error) {
    *error = NO_ERROR;

    uint8_t block_number = i_node_data->file_size / 32;
    if (block_number > new_block_size) {
        i_node__remove_blocks_from_end(i_node_data, block_number - new_block_size, error);
    } else if (block_number < new_block_size) {
        i_node__add_blocks_to_end(i_node_data, new_block_size - block_number, error);
    }
}