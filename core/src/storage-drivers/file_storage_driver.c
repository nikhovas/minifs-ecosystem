#include <minifs/storage-drivers/file_storage_driver.h>
#include <minifs/errors.h>
#include <unistd.h>
#include <minifs/constants.h>
#include <minifs/core/superblock.h>
#include <minifs/core/directory.h>
#include <minifs/filesystem_context.h>
#include <minifs/std-wrapper/memory.h>
#include <minifs/std-wrapper/string.h>


void create_file(char * path, int * error) {
    *error = NO_ERROR;

    FILE *fp = fopen(path, "wb");
    if (fp == NULL) {
        *error = FILE_ERROR;
        return;
    }

    if (fseek(fp, MINIFS__END_OFFSET - 1, SEEK_SET) != 0) {
        *error = FILE_ERROR;
        return;
    }

    fputc('\0', fp);
    if (ferror(fp)) {
        *error = FILE_ERROR;
        return;
    }

    fclose(fp);
}


void storage_set_part(struct minifs_core__filesystem_context * ctx, const uint8_t *data, size_t offset, size_t length, int *error) {
    struct file_storage_driver * fsd = (struct file_storage_driver *) &ctx->sdi;

    *error = NO_ERROR;

    if (fseek(fsd->fileptr, offset, SEEK_SET) != 0) {
        *error = FILE_ERROR;
        return;
    }

    if (fwrite(data, length, 1, fsd->fileptr) == 0) {
        *error = FILE_ERROR;
        return;
    }
}


void storage_get_part(struct minifs_core__filesystem_context * ctx, uint8_t *data, size_t offset, size_t length, int *error) {
    struct file_storage_driver * fsd = (struct file_storage_driver *) &ctx->sdi;

    *error = NO_ERROR;

    if (fseek(fsd->fileptr, offset, SEEK_SET) != 0) {
        *error = FILE_ERROR;
        return;
    }

    if (fread(data, length, 1, fsd->fileptr) == 0) {
        *error = FILE_ERROR;
        return;
    }
}


void storage_init(struct minifs_core__filesystem_context * ctx, int* error) {
    struct file_storage_driver * fsd = (struct file_storage_driver *) &ctx->sdi;

    if (access(fsd->file_path, F_OK) != 0) {
        create_file(fsd->file_path, error);
        fsd->fileptr = fopen(fsd->file_path, "r+b");

        superblock_data_t superblock_data;
        superblock_data.i_node_size = MINIFS__I_NODE_SIZE;
        superblock_data.block_size = MINIFS__BLOCK_SIZE;
        superblock_data.max_blocks = MINIFS__BLOCK_COUNT;
        superblock_data.max_i_nodes = MINIFS__I_NODE_COUNT;
        superblock_data.root_dir_id = dir_data_create(ctx, error);
        superblock__save(ctx, &superblock_data, error);
    } else {
        fsd->fileptr = fopen(fsd->file_path, "r+b");
    }
}


void storage_free(struct minifs_core__filesystem_context * ctx, int* error) {
    struct file_storage_driver * fsd = (struct file_storage_driver *) &ctx->sdi;
    fclose(fsd->fileptr);
}


struct file_storage_driver minifs_core__create_file_storage_driver(const char * file_path) {
    struct file_storage_driver fsd;
    fsd.set_part = &storage_set_part;
    fsd.get_part = &storage_get_part;
    fsd.init = &storage_init;
    fsd.free = &storage_free;
    fsd.fileptr = NULL;
    memcpy(fsd.file_path, file_path, strlen(file_path));
    return fsd;
}