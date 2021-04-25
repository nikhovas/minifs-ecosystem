#include "minifs/core/block.h"
#include "minifs/core/bitmap.h"
#include "minifs/constants.h"
#include "minifs/errors.h"


#ifdef KERNEL_MODULE
    #define fileptr NULL
#endif


block_data_t get_block_data_by_id(struct minifs_core__filesystem_context * ctx, uint8_t block_id, int *error) {
    block_data_t block_data;
    ctx->sdi->get_part(ctx, (uint8_t *) &block_data, MINIFS__BLOCK_DATA_OFFSET + block_id * MINIFS__BLOCK_SIZE, MINIFS__BLOCK_SIZE, error);
    return block_data;
}


void write_bock_data_by_id(struct minifs_core__filesystem_context * ctx, uint8_t block_id, block_data_t *block_data, int *error) {
    ctx->sdi->set_part(ctx, (uint8_t *) block_data, MINIFS__BLOCK_DATA_OFFSET + block_id * MINIFS__BLOCK_SIZE, MINIFS__BLOCK_SIZE, error);
}


uint8_t alloc_block(struct minifs_core__filesystem_context * ctx, int *error) {
    uint8_t id = alloc_bitmap_node(ctx, MINIFS__BLOCK_BITMAP_DATA_OFFSET, MINIFS__BLOCK_COUNT, error);
    if (*error == NO_FREE_NODES_AVAILABLE) {
        *error = BLOCKS_LIMIT;
        return 0;
    }
    return id;
}


void free_block(struct minifs_core__filesystem_context * ctx, uint8_t block_id, int *error) {
    free_bitmap_node(ctx, MINIFS__BLOCK_BITMAP_DATA_OFFSET, block_id, error);
}