#pragma once


#include <minifs/core/directory.h>
#include "errors.h"


void high_level__file_create(struct minifs_core__filesystem_context * ctx, const char * name, int * error);
void* high_level__file_get(struct minifs_core__filesystem_context * ctx, const char * name, int* data_length, int* error_code);
void high_level__file_write(struct minifs_core__filesystem_context * ctx, char* name, void* contents, int content_length, int * error);
void high_level__file_delete(struct minifs_core__filesystem_context * ctx, char * name, int * error);
void high_level__file_copy(struct minifs_core__filesystem_context * ctx, char * source, char * destination, int * error);


void high_level__dir_create(struct minifs_core__filesystem_context * ctx, char * name, int * error);
void high_level__dir_delete(struct minifs_core__filesystem_context * ctx, char * name, int * error);
directory_item_t * high_level__dir_get_contents(struct minifs_core__filesystem_context * ctx, char * name, uint8_t * size, int * error);