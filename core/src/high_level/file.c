#include "minifs/high_level.h"
#include <stdlib.h>
#include <string.h>
#include "minifs/core/file.h"
#include "minifs/core/directory.h"


#define ROOT_DIR_ID 0


void high_level__file_create(struct minifs_core__filesystem_context * ctx, const char * name, int * error) {
    char *name2 = malloc(strlen(name));
    strcpy(name2, name);

    uint8_t file_i_node_id = file__create(ctx, error);
    if (*error != NO_ERROR) {
        return;
    }

    dir_data_id__write_file_in_subdirs(ctx, ROOT_DIR_ID, name2, file_i_node_id, error);
    if (*error != 0) {
        file_id__remove(ctx, file_i_node_id, error);
        return;
    }
}


void* high_level__file_get(struct minifs_core__filesystem_context * ctx, const char * name, int* data_length, int* error_code) {
    char *name2 = malloc(strlen(name));
    strcpy(name2, name);

    found_file_info_t found_file = dir_data_id__get_file_in_subdirs(ctx, ROOT_DIR_ID, name2, 0, error_code);
    if (*error_code != 0) {
        return NULL;
    }

    int file_length = file_id__get_length(ctx, found_file.file_i_node_id, error_code);
    if (*error_code != 0) {
        return NULL;
    }

    uint8_t *contents = malloc(file_length);
    file_id__get_contents(ctx, found_file.file_i_node_id, (uint8_t *) contents, error_code);
    if (*error_code != 0) {
        return NULL;
    }

    *data_length = file_length;
    return contents;
}


void high_level__file_write(struct minifs_core__filesystem_context * ctx, char* name, void* contents, int content_length, int * error) {
    if (content_length > 256) {
        *error = TOO_BIG_FILE;
        return;
    }
    char *name2 = malloc(strlen(name));
    strcpy(name2, name);

    found_file_info_t found_file = dir_data_id__get_file_in_subdirs(ctx, ROOT_DIR_ID, name2, 0, error);
    if (*error != 0) {
        return;
    }

    file_id__overwrite(ctx, found_file.file_i_node_id, (uint8_t *) contents, content_length, error);
    if (*error != 0) {
        return;
    }
}


void high_level__file_delete(struct minifs_core__filesystem_context * ctx, char * name, int * error) {
    char *name2 = malloc(strlen(name));
    strcpy(name2, name);

    found_file_info_t found_file = dir_data_id__get_file_in_subdirs(ctx, ROOT_DIR_ID, name2, 0, error);
    if (*error != 0) {
        return;
    }

    dir_data_id__unregister_file(ctx, found_file.dir_i_node_id, name2, found_file.file_i_node_id, error);
    if (*error != 0) {
        return;
    }

    file_id__remove(ctx, found_file.file_i_node_id, error);
}


void high_level__file_copy(struct minifs_core__filesystem_context * ctx, char * source, char * destination, int * error) {
    char *name2 = malloc(strlen(source));
    strcpy(name2, source);

    found_file_info_t found_file = dir_data_id__get_file_in_subdirs(ctx, ROOT_DIR_ID, name2, 0, error);
    if (*error == NO_SUCH_FILE || *error == NOT_A_DIRECTORY || *error == NOT_A_VALID_PATH) {
        *error = NO_SUCH_SOURCE;
        return;
    } else if (*error != 0) {
        return;
    }

    int file_length = file_id__get_length(ctx, found_file.file_i_node_id, error);
    if (*error != 0) {
        return;
    }
    char *contents = malloc(file_length);
    file_id__get_contents(ctx, found_file.file_i_node_id, (uint8_t *) contents, error);
    if (*error != 0) {
        return;
    }

    high_level__file_create(ctx, destination, error);
    if (*error == NO_SUCH_FILE || *error == NOT_A_DIRECTORY || *error == NOT_A_VALID_PATH) {
        *error = NO_SUCH_DESTINATION;
        return;
    } else if (*error != 0) {
        return;
    }

    high_level__file_write(ctx, destination, contents, file_length, error);
    if (*error != 0) {
        high_level__file_delete(ctx, destination, error);
    }
}