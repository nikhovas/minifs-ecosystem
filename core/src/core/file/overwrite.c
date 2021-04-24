#include "minifs/core/file.h"
#include "minifs/core/block.h"
#include "minifs/core/i_node.h"
#include <minifs/std-wrapper/memory.h>


void file__overwrite(i_node_data_t *i_node_data, uint8_t* data, uint16_t length, int * error) {
    *error = NO_ERROR;

    uint8_t new_block_number = length / 32;
    uint8_t last_block_size = length % 32;
    if (last_block_size == 0) {
        last_block_size = 32;
    } else {
        ++new_block_number;
    }


    i_node__set_block_count(i_node_data, new_block_number, error);
    if (*error != NO_ERROR) {
        return;
    }

    i_node_data->file_size = length;

    block_data_t block_data;
    for (uint8_t i = 0; i < new_block_number - 1; ++i) {
        memcpy(&block_data, data + i * 32, 32);
        file__set_block_data(i_node_data, i, &block_data, error);
        if (*error != NO_ERROR) {
            return;
        }
    }

    memcpy(&block_data, data + (new_block_number - 1) * 32, last_block_size);
    file__set_block_data(i_node_data, new_block_number - 1, &block_data, error);
    if (*error != NO_ERROR) {
        return;
    }
}


void file_id__overwrite(uint8_t i_node_id, uint8_t* data, uint16_t length, int * error) {
    i_node_data_t file_i_node_data = i_node_id__get_data(i_node_id, error);
    if (*error != NO_ERROR) {
        return;
    }

    file__overwrite(&file_i_node_data, data, length, error);
    if (*error == NO_ERROR) {
        i_node__save(i_node_id, &file_i_node_data, error);
        if (*error != NO_ERROR) {
            return;
        }
    }
}