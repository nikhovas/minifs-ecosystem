#pragma once


#include <minifs/core/directory.h>
#include "errors.h"


void middle_level__file_create(struct minifs_core__filesystem_context * ctx, char * name, int * error);
void middle_level__file_get(struct minifs_core__filesystem_context * ctx, char * name, uint8_t* contents, int* data_length, int* error_code);
void middle_level__file_write(struct minifs_core__filesystem_context * ctx, char* name, void* contents, int content_length, int * error);
void middle_level__file_delete(struct minifs_core__filesystem_context * ctx, char * name, int * error);
void middle_level__file_copy(struct minifs_core__filesystem_context * ctx, char * source, char * destination, uint8_t * contents_meta_buffer, int * error);


void middle_level__dir_create(struct minifs_core__filesystem_context * ctx, char * name, int * error);
void middle_level__dir_delete(struct minifs_core__filesystem_context * ctx, char * name, int * error);
void middle_level__dir_get_contents(struct minifs_core__filesystem_context * ctx, char * name, directory_item_t * dir_items, uint8_t * size, int * error);