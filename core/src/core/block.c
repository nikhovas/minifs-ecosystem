#include "minifs/core/block.h"
#include "minifs/core/bitmap.h"
#include "minifs/disk.h"
#include "minifs/constants.h"
#include "minifs/errors.h"


#ifdef KERNEL_MODULE
    #define fileptr NULL
#endif


block_data_t get_block_data_by_id(uint8_t block_id, int *error) {
    block_data_t block_data;
    get_part_file((uint8_t *) &block_data, fileptr,  BLOCK_DATA_OFFSET+ block_id * BLOCK_SIZE, BLOCK_SIZE, error);
    return block_data;
}


void write_bock_data_by_id(uint8_t block_id, block_data_t *block_data, int *error) {
    set_part_file((uint8_t *) block_data, fileptr, BLOCK_DATA_OFFSET+ block_id * BLOCK_SIZE, BLOCK_SIZE, error);
}


uint8_t alloc_block(int *error) {
    uint8_t id = alloc_bitmap_node(fileptr, BLOCK_BITMAP_DATA_OFFSET, BLOCK_COUNT, error);
    if (*error == NO_FREE_NODES_AVAILABLE) {
        *error = BLOCKS_LIMIT;
        return 0;
    }
    return id;
}


void free_block(uint8_t block_id, int *error) {
    free_bitmap_node(fileptr, BLOCK_BITMAP_DATA_OFFSET, block_id, error);
}