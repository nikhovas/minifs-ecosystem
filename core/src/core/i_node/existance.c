#include "minifs/core/i_node.h"
#include "minifs/disk.h"
#include "minifs/core/bitmap.h"
#include "minifs/constants.h"
#include "minifs/errors.h"



#ifdef KERNEL_MODULE
    #define fileptr NULL
#endif


i_node_data_t i_node_id__get_data(struct minifs_core__filesystem_context * ctx, uint8_t i_node_id, int *error) {
    i_node_data_t i_node_data;
    ctx->sdi->get_part(ctx, (uint8_t *) &i_node_data, MINIFS__I_NODE_DATA_OFFSET + i_node_id * MINIFS__I_NODE_SIZE, MINIFS__I_NODE_SIZE, error);
    return i_node_data;
}


void i_node__save(struct minifs_core__filesystem_context * ctx, uint8_t i_node_id, i_node_data_t *i_node_data, int *error) {
    ctx->sdi->set_part(ctx, (uint8_t *) i_node_data, MINIFS__I_NODE_DATA_OFFSET + i_node_id * MINIFS__I_NODE_SIZE, MINIFS__I_NODE_SIZE, error);
}


uint8_t i_node__alloc(struct minifs_core__filesystem_context * ctx, int *error) {
    uint8_t id = alloc_bitmap_node(ctx, MINIFS__SUPERBLOCK_SIZE, 256, error);
    if (*error == NO_FREE_NODES_AVAILABLE) {
        *error = FILE_LIMIT;
        return 0;
    }
    return id;
}


void i_node_id__free(struct minifs_core__filesystem_context * ctx, uint8_t i_node_id, int *error) {
    free_bitmap_node(ctx, MINIFS__SUPERBLOCK_SIZE, i_node_id, error);
}