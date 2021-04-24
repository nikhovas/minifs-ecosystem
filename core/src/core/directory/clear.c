#include "minifs/core/directory.h"
#include "minifs/core/file.h"


void dir_data_clear(struct minifs_core__filesystem_context * ctx, i_node_data_t *dir_i_node_data, int *error) {
    *error = NO_ERROR;

    uint8_t blocks = dir_i_node_data->file_size / 32;
    uint8_t last_block_size = dir_i_node_data->file_size % 32;
    if (last_block_size == 0) {
        last_block_size = 32;
    }

    if (blocks == 0) {
        return;
    }

    for (uint8_t i = 0; i < blocks - 1; ++i) {
        directory_data_t dir_data = dir_data_get_by_id(ctx, dir_i_node_data->block_ids[i], error);
        if (*error != NO_ERROR) {
            return;
        }

        for (uint8_t j = 0; j < 4; ++j) {
            i_node_data_t content_i_node = i_node_id__get_data(ctx, dir_data.dir_items[j].i_node_id, error);
            if (*error !=NO_ERROR) {
                return;
            }

            if (content_i_node.is_folder) {
                i_node_data_t del_dir_i_node_data = i_node_id__get_data(ctx, dir_data.dir_items[j].i_node_id, error);
                if (*error != NO_ERROR) {
                    return;
                }

                dir_data__delete(ctx, &del_dir_i_node_data, dir_data.dir_items[j].i_node_id, error);
                if (*error != NO_ERROR) {
                    return;
                }
            } else {
                file_id__remove(ctx, dir_data.dir_items[j].i_node_id, error);
                if (*error != NO_ERROR) {
                    return;
                }
            }
        }
        free_block(ctx, dir_i_node_data->block_ids[i], error);
        if (*error != NO_ERROR) {
            return;
        }

        dir_i_node_data->block_ids[i] = 0;
    }

    directory_data_t dir_data = dir_data_get_by_id(ctx, dir_i_node_data->block_ids[blocks - 1], error);
    if (*error != NO_ERROR) {
        return;
    }

    for (uint8_t j = 0; j < last_block_size / 8; ++j) {
        i_node_data_t content_i_node = i_node_id__get_data(ctx, dir_data.dir_items[j].i_node_id, error);
        if (*error != NO_ERROR) {
            return;
        }

        if (content_i_node.is_folder) {
            i_node_data_t del_dir_i_node_data = i_node_id__get_data(ctx, dir_data.dir_items[j].i_node_id, error);
            if (*error != NO_ERROR) {
                return;
            }

            dir_data__delete(ctx, &del_dir_i_node_data, dir_data.dir_items[j].i_node_id, error);
            if (*error != NO_ERROR) {
                return;
            }
        } else {
            file_id__remove(ctx, dir_data.dir_items[j].i_node_id, error);
            if (*error != NO_ERROR) {
                return;
            }
        }
    }
    free_block(ctx, dir_i_node_data->block_ids[blocks - 1], error);
    if (*error != NO_ERROR) {
        return;
    }

    dir_i_node_data->block_ids[blocks - 1] = 0;
}