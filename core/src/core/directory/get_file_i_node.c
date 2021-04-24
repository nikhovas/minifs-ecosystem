#include "minifs/core/directory.h"
#include <minifs/std-wrapper/string.h>
#include "minifs/errors.h"


uint8_t dir_data_get_file_i_node(struct minifs_core__filesystem_context * ctx, i_node_data_t *dir_i_node_data, char *filename, uint8_t file_i_node_id, int *error) {
    *error = NO_ERROR;

    if (dir_i_node_data->is_folder == 0) {
        *error = NOT_A_DIRECTORY;
        return 0;
    }

    for (uint8_t i = 0; i < 6 && i * 4 * 8 < dir_i_node_data->file_size; ++i) {
        directory_data_t dir_data = dir_data_get_by_id(ctx, dir_i_node_data->block_ids[i], error);
        if (*error != NO_ERROR) {
            return 0;
        }
        for (uint8_t j = 0; j < 4 && (i * 4 + j) * 8 < dir_i_node_data->file_size; ++j) {
            if (strcmp(dir_data.dir_items[j].name, filename) == 0 || dir_data.dir_items[j].i_node_id == file_i_node_id) {
                return dir_data.dir_items[j].i_node_id;
            }
        }
    }

    *error = NO_SUCH_FILE;
    return 0;
}
