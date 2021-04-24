#include "minifs/high_level.h"
#include <stdlib.h>
#include <string.h>
#include "minifs/core/directory.h"


#define ROOT_DIR_ID 0


void high_level__dir_create(struct minifs_core__filesystem_context * ctx, char * name, int * error) {
    char *name2 = malloc(strlen(name));
    strcpy(name2, name);

    uint8_t new_dir_i_node_id = dir_data_create(ctx, error);
    if (*error != 0) {
        return;
    }

    dir_data_id__write_file_in_subdirs(ctx, ROOT_DIR_ID, name2, new_dir_i_node_id, error);
    if (*error == FILE_ALREADY_EXISTS) {
        i_node_id__free(ctx, new_dir_i_node_id, error);
        return;
    } else if (*error != 0) {
        dir_data_id__delete(ctx, new_dir_i_node_id, error);
        if (*error != 0) {
            return;
        }
        return;
    }
}

void high_level__dir_delete(struct minifs_core__filesystem_context * ctx, char * name, int * error) {
    char *name2 = malloc(strlen(name));
    strcpy(name2, name);

    found_file_info_t found_file = dir_data_id__get_file_in_subdirs(ctx, ROOT_DIR_ID, name2, 0, error);
    if (*error != 0) {
        return;
    }

    dir_data_id__unregister_file(ctx, found_file.dir_i_node_id, name, found_file.file_i_node_id, error);
    dir_data_id__delete(ctx, found_file.file_i_node_id, error);
    if (*error != NO_ERROR) {
        return;
    }
}


directory_item_t * high_level__dir_get_contents(struct minifs_core__filesystem_context * ctx, char * name, uint8_t * size, int * error) {
    char *name2 = malloc(strlen(name));
    strcpy(name2, name);

    found_file_info_t found_file = dir_data_id__get_file_in_subdirs(ctx, ROOT_DIR_ID, name2, 0, error);
    if (*error != 0) {
        return NULL;
    }

    *size = dir_data_id__get_number_of_files(ctx, found_file.file_i_node_id, error);
    directory_item_t *dir_items = malloc((*size) * sizeof(directory_item_t));
    dir_data_id__get_files_list(ctx, found_file.file_i_node_id, dir_items, error);
    return dir_items;
}