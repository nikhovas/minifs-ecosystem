#include "minifs/core/file.h"


block_data_t file__get_block_data(struct minifs_core__filesystem_context * ctx, i_node_data_t *i_node_data, uint8_t block_number, int *error) {
    uint8_t block_id = i_node_data->block_ids[block_number];
    return get_block_data_by_id(ctx, block_id, error);
}


void file__set_block_data(struct minifs_core__filesystem_context * ctx, i_node_data_t *i_node_data, uint8_t block_number, block_data_t* block_data, int *error) {
    uint8_t block_id = i_node_data->block_ids[block_number];
    write_bock_data_by_id(ctx, block_id, block_data, error);
}