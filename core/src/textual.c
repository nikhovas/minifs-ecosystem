#include <stdio.h>
#include <malloc.h>
#include "minifs/textual.h"
#include "minifs/high_level.h"


void minifs_core_textual__file_create(const char * name, int * error) {
    high_level__file_create(name, error);
}


void* minifs_core_textual__file_get(const char * name, int* data_length, int* error_code) {
    return high_level__file_get(name, data_length, error_code);
}


void minifs_core_textual__file_write(char* name, void* contents, int content_length, int * error) {
    high_level__file_write(name, contents, content_length, error);
}


void minifs_core_textual__file_delete(char * name, int * error) {
    high_level__file_delete(name, error);
}


void minifs_core_textual__file_copy(char * source, char * destination, int * error) {
    high_level__file_copy(source, destination, error);
}


void minifs_core_textual__dir_create(char * name, int * error) {
    high_level__dir_create(name, error);
}


void minifs_core_textual__dir_delete(char * name, int * error) {
    high_level__dir_delete(name, error);
}


char* minifs_core_textual__dir_get_contents(char * name, int* data_length, int* error) {
    uint8_t size;
    directory_item_t * dir_items = high_level__dir_get_contents(name, &size, error);
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