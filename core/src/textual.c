#include <stdio.h>
#include <malloc.h>
#include "minifs/textual.h"
#include "minifs/high_level.h"


void minifs_core_textual__file_create(struct minifs_core__filesystem_context * ctx, const char * name, int * error) {
    high_level__file_create(ctx, name, error);
}


void* minifs_core_textual__file_get(struct minifs_core__filesystem_context * ctx, const char * name, int* data_length, int* error_code) {
    return high_level__file_get(ctx, name, data_length, error_code);
}


void minifs_core_textual__file_write(struct minifs_core__filesystem_context * ctx, char* name, void* contents, int content_length, int * error) {
    high_level__file_write(ctx, name, contents, content_length, error);
}


void minifs_core_textual__file_delete(struct minifs_core__filesystem_context * ctx, char * name, int * error) {
    high_level__file_delete(ctx, name, error);
}


void minifs_core_textual__file_copy(struct minifs_core__filesystem_context * ctx, char * source, char * destination, int * error) {
    high_level__file_copy(ctx, source, destination, error);
}


void minifs_core_textual__dir_create(struct minifs_core__filesystem_context * ctx, char * name, int * error) {
    high_level__dir_create(ctx, name, error);
}


void minifs_core_textual__dir_delete(struct minifs_core__filesystem_context * ctx, char * name, int * error) {
    high_level__dir_delete(ctx, name, error);
}


char* minifs_core_textual__dir_get_contents(struct minifs_core__filesystem_context * ctx, char * name, int* data_length, int* error) {
    uint8_t size;
    directory_item_t * dir_items = high_level__dir_get_contents(ctx, name, &size, error);
    char * result = malloc(128);
    int result_size = 0;
    if (*error == NO_ERROR) {
        for (uint8_t i = 0; i < size; ++i) {
            result_size += sprintf(result + result_size, "%s\n", dir_items[i].name);
        }
        *data_length = result_size;
    }
    return result;
}