#ifdef KERNEL_MODULE
    #include <linux/string.h>
#else
    #include <memory.h>
#endif
#include "minifs/core/file.h"


void file__get_contents(i_node_data_t *i_node_data, uint8_t* contents, int *error) {
    i_node_block_info_t block_info = get_i_node_block_info(i_node_data);

    uint8_t i = 0;
    for (; i < block_info.number_of_blocks - 1; ++i) {
        block_data_t block_data = file__get_block_data(i_node_data, i, error);
        if (*error != NO_ERROR) {
            return;
        }

        memcpy(contents + i * 32, &block_data, 32);
    }

    block_data_t block_data = file__get_block_data(i_node_data, block_info.number_of_blocks - 1, error);
    if (*error != NO_ERROR) {
        return;
    }

    memcpy(contents + (block_info.number_of_blocks - 1) * 32, &block_data, block_info.last_block_size);
}

void file_id__get_contents(uint8_t i_node_id, uint8_t* contents, int *error) {
    i_node_data_t i_node_data = i_node_id__get_data(i_node_id, error);
    if (*error != NO_ERROR) {
        return;
    }

    file__get_contents(&i_node_data, contents, error);
    if (*error != NO_ERROR) {
        return;
    }
}

