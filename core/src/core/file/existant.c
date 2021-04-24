#include "minifs/core/file.h"


uint8_t file__create(struct minifs_core__filesystem_context * ctx, int *error) {
    *error = NO_ERROR;
    uint8_t i_node_id = i_node__alloc(ctx, error);
    if (*error != NO_ERROR) {
        return 0;
    }

    i_node_data_t i_node_data;
    i_node_data.file_size = 0;
    i_node_data.is_folder = 0;
    i_node__save(ctx, i_node_id, &i_node_data, error);
    if (*error != NO_ERROR) {
        return 0;
    }

    return i_node_id;
}


void file_id__remove(struct minifs_core__filesystem_context * ctx, uint8_t i_node_id, int *error) {
    i_node_data_t i_node_data = i_node_id__get_data(ctx, i_node_id, error);
    if (*error != NO_ERROR) {
        return;
    }

    uint8_t block_count = i_node_data.file_size / 32;
    if (i_node_data.file_size % 32 != 0) {
        ++block_count;
    }
    i_node__remove_blocks_from_end(ctx, &i_node_data, block_count, error);
    if (*error != NO_ERROR) {
        return;
    }

    i_node_id__free(ctx, i_node_id, error);
}