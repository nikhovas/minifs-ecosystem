#pragma once


#include <minifs/std-wrapper/stdint.h>
#include <stdio.h>
#include <minifs/filesystem_context.h>


struct file_storage_driver {
    // interface declarations
    void (*set_part)(struct minifs_core__filesystem_context * ctx, const uint8_t *data, size_t offset, size_t length, int *error);
    void (*get_part)(struct minifs_core__filesystem_context * ctx, uint8_t *data, size_t offset, size_t length, int *error);
    void (*init)(struct minifs_core__filesystem_context * ctx, int* error);
    void (*free)(struct minifs_core__filesystem_context * ctx, int* error);
    // own data
    char file_path[256];
    FILE * fileptr;
};


struct file_storage_driver minifs_core__create_file_storage_driver(const char * file_path);