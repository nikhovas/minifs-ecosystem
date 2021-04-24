#include "minifs/core/i_node.h"


i_node_block_info_t get_i_node_block_info(i_node_data_t *i_node_data) {
    i_node_block_info_t i_node_block_info;
    i_node_block_info.number_of_blocks = i_node_data->file_size / 32;
    i_node_block_info.last_block_size = i_node_data->file_size % 32;
    if (i_node_block_info.last_block_size == 0) {
        i_node_block_info.last_block_size = 32;
    } else {
        ++i_node_block_info.number_of_blocks;
    }
    return i_node_block_info;
}