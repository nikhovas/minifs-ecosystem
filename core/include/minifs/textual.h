#pragma once
#include <minifs/filesystem_context.h>


void minifs_core_textual__file_create(struct minifs_core__filesystem_context * ctx, const char * name, int * error);
void* minifs_core_textual__file_get(struct minifs_core__filesystem_context * ctx, const char * name, int* data_length, int* error_code);
void minifs_core_textual__file_write(struct minifs_core__filesystem_context * ctx, char* name, void* contents, int content_length, int * error);
void minifs_core_textual__file_delete(struct minifs_core__filesystem_context * ctx, char * name, int * error);
void minifs_core_textual__file_copy(struct minifs_core__filesystem_context * ctx, char * source, char * destination, int * error);


void minifs_core_textual__dir_create(struct minifs_core__filesystem_context * ctx, char * name, int * error);
void minifs_core_textual__dir_delete(struct minifs_core__filesystem_context * ctx, char * name, int * error);
char* minifs_core_textual__dir_get_contents(struct minifs_core__filesystem_context * ctx, char * name, int* data_length, int*error);