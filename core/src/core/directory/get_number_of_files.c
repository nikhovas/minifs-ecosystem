#include "minifs/core/directory.h"


uint8_t dir_data__get_number_of_files(struct minifs_core__filesystem_context * ctx, const i_node_data_t *dir_i_node_data, int * error) {
    return dir_i_node_data->file_size / 8;
}

uint8_t dir_data_id__get_number_of_files(struct minifs_core__filesystem_context * ctx, uint8_t dir_i_node_id, int * error) {
    i_node_data_t dir_i_node_data = i_node_id__get_data(ctx, dir_i_node_id, error);
    if (*error != NO_ERROR) {
        return 0;
    }

    return dir_data__get_number_of_files(ctx, &dir_i_node_data, error);
}
