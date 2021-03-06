#pragma once


#include <minifs/std-wrapper/stdint.h>
#include <minifs/filesystem_context.h>


typedef struct __attribute__((packed)) {
    uint64_t i_node_size;
    uint64_t block_size;
    uint64_t root_dir_id;
    uint64_t max_i_nodes;
    uint64_t max_blocks;
} superblock_data_t;


void superblock__save(struct minifs_core__filesystem_context * ctx, superblock_data_t *superblock_data, int *error);