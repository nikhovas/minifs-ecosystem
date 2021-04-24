#include <minifs/storage-drivers/file_storage_driver.h>
#include <stdio.h>

// struct file_storage_driver {
//     // interface declarations
//     void (*set_part)(struct storage_driver_interface * sdi, const uint8_t *data, size_t offset, size_t length, int *error);
//     void (*get_part)(struct storage_driver_interface * sdi, uint8_t *data, size_t offset, size_t length, int *error);
//     // own data
//     FILE * fileptr;
// };


void set_part(struct storage_driver_interface * sdi, const uint8_t *data, size_t offset, size_t length, int *error) {
    struct file_storage_driver * fsd = (struct file_storage_driver *) sdi;

    *error = NO_ERROR;

    if (fseek(file_storage_driver->fileptr, offset, SEEK_SET) != 0) {
        *error = FILE_ERROR;
        return;
    }

    if (fwrite(data, length, 1, file_storage_driver->fileptr) == 0) {
        *error = FILE_ERROR;
        return;
    }
}


void get_part(struct storage_driver_interface * sdi, uint8_t *data, size_t offset, size_t length, int *error) {
    struct file_storage_driver * fsd = (struct file_storage_driver *) sdi;

    *error = NO_ERROR;

    if (fseek(file_storage_driver->fileptr, offset, SEEK_SET) != 0) {
        *error = FILE_ERROR;
        return;
    }

    if (fread(data, length, 1, file_storage_driver->fileptr) == 0) {
        *error = FILE_ERROR;
        return;
    }
}


void init(struct storage_driver_interface * sdi, int* error) {
    struct file_storage_driver * fsd = (struct file_storage_driver *) sdi;
}


void free(struct storage_driver_interface * sdi, int* error) {
    struct file_storage_driver * fsd = (struct file_storage_driver *) sdi;

    if (access(file_storage_driver, F_OK) != 0) {
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


struct file_storage_driver create_file_storage_driver(const char * file_path) {
    struct file_storage_driver fsd;

}