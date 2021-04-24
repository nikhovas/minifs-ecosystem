#include "minifs/core/directory.h"
#include <minifs/std-wrapper/memory.h>


void dir_data__get_files_list(struct minifs_core__filesystem_context * ctx, i_node_data_t *dir_i_node_data, directory_item_t* dir_items, int * error) {
    *error = NO_ERROR;

    if (dir_i_node_data->is_folder == 0) {
        *error = NOT_A_DIRECTORY;
        return;
    }

    for (uint8_t i = 0; i < 6 && i * 4 * 8 < dir_i_node_data->file_size; ++i) {
        directory_data_t dir_data = dir_data_get_by_id(ctx, dir_i_node_data->block_ids[i], error);
        if (*error != NO_ERROR) {
            return;
        }

        for (uint8_t j = 0; j < 4 && i * 4 * 8 + j * 8 < dir_i_node_data->file_size; ++j) {
            memcpy(&dir_items[i * 4 + j], &dir_data.dir_items[j], 8);
        }
    }
}

void dir_data_id__get_files_list(struct minifs_core__filesystem_context * ctx, uint8_t dir_i_node_id, directory_item_t* dir_items, int * error) {
    i_node_data_t dir_i_node_data = i_node_id__get_data(ctx, dir_i_node_id, error);
    if (*error != NO_ERROR) {
        return;
    }

    dir_data__get_files_list(ctx, &dir_i_node_data, dir_items, error);
}