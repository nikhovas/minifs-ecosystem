#include <minifs/storage-drivers/in_memory_storage_driver.h>
#include <minifs/errors.h>
#include <unistd.h>
#include <minifs/constants.h>
#include <minifs/core/superblock.h>
#include <minifs/core/directory.h>
#include <minifs/filesystem_context.h>
#include <minifs/std-wrapper/string.h>


void in_memory_storage_set_part(struct minifs_core__filesystem_context * ctx, const uint8_t *data, size_t offset, size_t length, int *error) {
    *error = NO_ERROR;
    struct in_memory_storage_driver * imsd = (struct in_memory_storage_driver *) ctx->sdi;
    if (offset + length >= MINIFS__END_OFFSET) {
        *error = FILE_ERROR;
        return;
    }

    memcpy(imsd->buffer + offset, data, length);
}


void in_memory_storage_get_part(struct minifs_core__filesystem_context * ctx, uint8_t *data, size_t offset, size_t length, int *error) {
    *error = NO_ERROR;
    struct in_memory_storage_driver * imsd = (struct in_memory_storage_driver *) ctx->sdi;
    if (offset + length >= MINIFS__END_OFFSET) {
        *error = FILE_ERROR;
        return;
    }

    memcpy(data, imsd->buffer + offset, length);
}


void in_memory_storage_init(struct minifs_core__filesystem_context * ctx, int* error) {
//    struct in_memory_storage_driver * imsd = (struct in_memory_storage_driver *) ctx->sdi;

    superblock_data_t superblock_data;
    superblock_data.i_node_size = MINIFS__I_NODE_SIZE;
    superblock_data.block_size = MINIFS__BLOCK_SIZE;
    superblock_data.max_blocks = MINIFS__BLOCK_COUNT;
    superblock_data.max_i_nodes = MINIFS__I_NODE_COUNT;
    superblock_data.root_dir_id = dir_data_create(ctx, error);
    superblock__save(ctx, &superblock_data, error);
}


void in_memory_storage_free(struct minifs_core__filesystem_context * ctx, int* error) {}


void minifs_core__create_in_memory_storage_driver(struct in_memory_storage_driver * imsd) {
    imsd->set_part = &in_memory_storage_set_part;
    imsd->get_part = &in_memory_storage_get_part;
    imsd->init = &in_memory_storage_init;
    imsd->free = &in_memory_storage_free;
}