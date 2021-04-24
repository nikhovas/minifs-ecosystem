#include "minifs/middle_level.h"
// #include <stdlib.h>
// #include <string.h>
#include "minifs/core/file.h"
#include "minifs/core/directory.h"


#define ROOT_DIR_ID 0


void middle_level__file_create(char * name, int * error) {
    uint8_t file_i_node_id = file__create(error);
    if (*error != NO_ERROR) {
        return;
    }

    dir_data_id__write_file_in_subdirs(ROOT_DIR_ID, name, file_i_node_id, error);
    if (*error != 0) {
        file_id__remove(file_i_node_id, error);
        return;
    }
}


void middle_level__file_get(char * name, uint8_t* contents, int* data_length, int* error_code) {
    found_file_info_t found_file = dir_data_id__get_file_in_subdirs(ROOT_DIR_ID, name, 0, error_code);
    if (*error_code != 0) {
        return;
    }

    int file_length = file_id__get_length(found_file.file_i_node_id, error_code);
    if (*error_code != 0) {
        return;
    }

    file_id__get_contents(found_file.file_i_node_id, contents, error_code);
    if (*error_code != 0) {
        return;
    }

    *data_length = file_length;
}


void middle_level__file_write(char* name, void* contents, int content_length, int * error) {
    if (content_length > 256) {
        *error = TOO_BIG_FILE;
        return;
    }

    found_file_info_t found_file = dir_data_id__get_file_in_subdirs(ROOT_DIR_ID, name, 0, error);
    if (*error != 0) {
        return;
    }

    file_id__overwrite(found_file.file_i_node_id, (uint8_t *) contents, content_length, error);
    if (*error != 0) {
        return;
    }
}


void middle_level__file_delete(char * name, int * error) {
    found_file_info_t found_file = dir_data_id__get_file_in_subdirs(ROOT_DIR_ID, name, 0, error);
    if (*error != 0) {
        return;
    }

    dir_data_id__unregister_file(found_file.dir_i_node_id, name, found_file.file_i_node_id, error);
    if (*error != 0) {
        return;
    }

    file_id__remove(found_file.file_i_node_id, error);
}


void middle_level__file_copy(char * source, char * destination, uint8_t * contents_meta_buffer, int * error) {
    found_file_info_t found_file = dir_data_id__get_file_in_subdirs(ROOT_DIR_ID, source, 0, error);
    if (*error == NO_SUCH_FILE || *error == NOT_A_DIRECTORY || *error == NOT_A_VALID_PATH) {
        *error = NO_SUCH_SOURCE;
        return;
    } else if (*error != 0) {
        return;
    }

    int file_length = file_id__get_length(found_file.file_i_node_id, error);
    if (*error != 0) {
        return;
    }

    file_id__get_contents(found_file.file_i_node_id, contents_meta_buffer, error);
    if (*error != 0) {
        return;
    }

    middle_level__file_create(destination, error);
    if (*error == NO_SUCH_FILE || *error == NOT_A_DIRECTORY || *error == NOT_A_VALID_PATH) {
        *error = NO_SUCH_DESTINATION;
        return;
    } else if (*error != 0) {
        return;
    }

    middle_level__file_write(destination, contents_meta_buffer, file_length, error);
    if (*error != 0) {
        middle_level__file_delete(destination, error);
    }
}