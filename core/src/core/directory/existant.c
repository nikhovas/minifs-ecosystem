#include "minifs/core/directory.h"
#include "minifs/core/block.h"


uint8_t dir_data_create(struct minifs_core__filesystem_context * ctx, int *error) {
    *error = NO_ERROR;

    uint8_t i_node_id = i_node__alloc(ctx, error);
    if (*error != NO_ERROR) {
        return 0;
    }

    i_node_data_t i_node_data;
    i_node_data.file_size = 0;
    i_node_data.is_folder = 1;
    for (uint8_t i = 0; i < 6; ++i) {
        i_node_data.block_ids[i] = 0;
    }

    i_node__save(ctx, i_node_id, &i_node_data, error);
    if (*error != NO_ERROR) {
        return 0;
    }

    return i_node_id;
}


void dir_data__delete(struct minifs_core__filesystem_context * ctx, i_node_data_t *dir_i_node_data, uint8_t dir_i_node_id, int *error) {
    *error = NO_ERROR;

    dir_data_clear(ctx, dir_i_node_data, error);
    if (*error != NO_ERROR) {
        return;
    }

    i_node_id__free(ctx, dir_i_node_id, error);
}

void dir_data_id__delete(struct minifs_core__filesystem_context * ctx, uint8_t dir_i_node_id, int *error) {
    i_node_data_t dir_i_node_data = i_node_id__get_data(ctx, dir_i_node_id, error);
    if (*error != NO_ERROR) {
        return;
    }

    dir_data__delete(ctx, &dir_i_node_data, dir_i_node_id, error);
    if (*error != NO_ERROR) {
        return;
    }
}


directory_data_t dir_data_get_by_id(struct minifs_core__filesystem_context * ctx, uint8_t dir_data_id, int *error) {
    block_data_t block_data = get_block_data_by_id(ctx, dir_data_id, error);
    if (*error != NO_ERROR) {
        return (directory_data_t) {0};
    }

    directory_data_t *dir_data = (directory_data_t*) &block_data;
    return *dir_data;
}


void dir_data_save_by_id(struct minifs_core__filesystem_context * ctx, uint8_t dir_data_id, directory_data_t *block_data, int *error) {
    write_bock_data_by_id(ctx, dir_data_id, (block_data_t *) block_data, error);
}