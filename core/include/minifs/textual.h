#pragma once


void minifs_core_textual__file_create(const char * name, int * error);
void* minifs_core_textual__file_get(const char * name, int* data_length, int* error_code);
void minifs_core_textual__file_write(char* name, void* contents, int content_length, int * error);
void minifs_core_textual__file_delete(char * name, int * error);
void minifs_core_textual__file_copy(char * source, char * destination, int * error);


void minifs_core_textual__dir_create(char * name, int * error);
void minifs_core_textual__dir_delete(char * name, int * error);
char* minifs_core_textual__dir_get_contents(char * name, int* data_length, int*error);