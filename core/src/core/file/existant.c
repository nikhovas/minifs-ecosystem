#include "minifs/core/file.h"


uint8_t file__create(int *error) {
    *error = NO_ERROR;
    uint8_t i_node_id = i_node__alloc(error);
    if (*error != NO_ERROR) {
        return 0;
    }

    i_node_data_t i_node_data;
    i_node_data.file_size = 0;
    i_node_data.is_folder = 0;
    i_node__save(i_node_id, &i_node_data, error);
    if (*error != NO_ERROR) {
        return 0;
    }

    return i_node_id;
}


void file_id__remove(uint8_t i_node_id, int *error) {
    i_node_data_t i_node_data = i_node_id__get_data(i_node_id, error);
    if (*error != NO_ERROR) {
        return;
    }

    uint8_t block_count = i_node_data.file_size / 32;
    if (i_node_data.file_size % 32 != 0) {
        ++block_count;
    }
    i_node__remove_blocks_from_end(&i_node_data, block_count, error);
    if (*error != NO_ERROR) {
        return;
    }

    i_node_id__free(i_node_id, error);
}