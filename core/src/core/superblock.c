#include "minifs/core/superblock.h"
#include "minifs/constants.h"
#include "minifs/disk.h"


#ifdef KERNEL_MODULE
    #define fileptr NULL
#endif


void superblock__save(superblock_data_t *superblock_data, int *error) {
    set_part_file((uint8_t *) superblock_data, fileptr, SUPERBLOCK_DATA_OFFSET, SUPERBLOCK_SIZE, error);
}