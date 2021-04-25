#include "minifs/core/superblock.h"
#include "minifs/constants.h"


#ifdef KERNEL_MODULE
    #define fileptr NULL
#endif


void superblock__save(struct minifs_core__filesystem_context * ctx, superblock_data_t *superblock_data, int *error) {
    ctx->sdi->set_part(ctx, (uint8_t *) superblock_data, MINIFS__SUPERBLOCK_DATA_OFFSET, MINIFS__SUPERBLOCK_SIZE, error);
}