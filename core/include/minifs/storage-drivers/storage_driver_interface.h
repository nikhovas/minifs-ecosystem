#pragma once
#include <minifs/std-wrapper/stdint.h>


struct minifs_core__filesystem_context;


struct storage_driver_interface {
    void (*set_part)(struct minifs_core__filesystem_context * ctx, const uint8_t *data, size_t offset, size_t length, int *error);
    void (*get_part)(struct minifs_core__filesystem_context * ctx, uint8_t *data, size_t offset, size_t length, int *error);
    void (*init)(struct minifs_core__filesystem_context * ctx, int* error);
    void (*free)(struct minifs_core__filesystem_context * ctx, int* error);
};