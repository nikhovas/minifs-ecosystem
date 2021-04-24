#include "minifs/middle_level.h"
#include <minifs/std-wrapper/string.h>
#include "minifs/core/directory.h"


#define ROOT_DIR_ID 0


void middle_level__dir_create(char * name, int * error) {
    uint8_t new_dir_i_node_id = dir_data_create(error);
    if (*error != 0) {
        return;
    }

    dir_data_id__write_file_in_subdirs(ROOT_DIR_ID, name, new_dir_i_node_id, error);
    if (*error != 0) {
        dir_data_id__delete(new_dir_i_node_id, error);
        if (*error != 0) {
            return;
        }

        return;
    }
}

void middle_level__dir_delete(char * name, int * error) {
    found_file_info_t found_file = dir_data_id__get_file_in_subdirs(ROOT_DIR_ID, name, 0, error);
    if (*error != 0) {
        return;
    }

    dir_data_id__unregister_file(found_file.dir_i_node_id, name, found_file.file_i_node_id, error);
    dir_data_id__delete(found_file.file_i_node_id, error);
    if (*error != NO_ERROR) {
        return;
    }
}


void middle_level__dir_get_contents(char * name, directory_item_t *dir_items, uint8_t * size, int * error) {
    found_file_info_t found_file = dir_data_id__get_file_in_subdirs(ROOT_DIR_ID, name, 0, error);
    if (*error != 0) {
        return;
    }

    *size = dir_data_id__get_number_of_files(found_file.file_i_node_id, error);
    dir_data_id__get_files_list(found_file.file_i_node_id, dir_items, error);
    return;
}