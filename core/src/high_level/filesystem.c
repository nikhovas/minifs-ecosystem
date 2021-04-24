#include "minifs/high_level.h"
#include <stdlib.h>
#include <string.h>
#include "minifs/core/file.h"
#include "minifs/core/directory.h"
#include "minifs/disk.h"
#include <unistd.h>
#include "minifs/core/superblock.h"
#include "minifs/constants.h"


#define ROOT_DIR_ID 0


void high_level__filesystem_create(char * filename, int *error) {
    if (access(filename, F_OK) != 0) {
        create_file(filename, error);
        set_file(fopen(filename, "r+b"));

        superblock_data_t superblock_data;
        superblock_data.i_node_size = I_NODE_SIZE;
        superblock_data.block_size = BLOCK_SIZE;
        superblock_data.max_blocks = BLOCK_COUNT;
        superblock_data.max_i_nodes = I_NODE_COUNT;
        superblock_data.root_dir_id = dir_data_create(error);
        superblock__save(&superblock_data, error);
    } else {
        set_file(fopen(filename, "r+b"));
    }
}