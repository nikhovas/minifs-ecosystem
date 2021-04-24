#pragma once



#ifdef KERNEL_MODULE
    #include <linux/types.h>
    #define FILE void
#else
    #include <unistd.h>
    #include <stdio.h>
    #include <stdint.h>
#endif
#include <minifs/filesystem_context.h>


size_t alloc_bitmap_node(struct minifs_core__filesystem_context * ctx, size_t offset, size_t length, int * error);
void free_bitmap_node(struct minifs_core__filesystem_context * ctx, size_t offset, size_t number, int *error);