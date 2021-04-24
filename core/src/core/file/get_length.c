#include "minifs/core/file.h"


uint16_t file__get_length(i_node_data_t *i_node_data) {
    return i_node_data->file_size;
}


uint16_t file_id__get_length(uint8_t i_node_id, int *error) {
    i_node_data_t i_node_data = i_node_id__get_data(i_node_id, error);
    if (*error != NO_ERROR) {
        return 0;
    }
    return file__get_length(&i_node_data);
}