#include "minifs/core/directory.h"
#include "minifs/core/block.h"
#include <minifs/std-wrapper/string.h>
#include "minifs/errors.h"


void dir_data__register_file_with_name(i_node_data_t *dir_i_node_data, char* filename, uint8_t file_node_id, int *error) {
    *error = NO_ERROR;

    if (strlen(filename) > 7) {
        *error = WRONG_FILE_NAME;
        return;
    }

    int local_error;
    dir_data_get_file_i_node(dir_i_node_data, filename, file_node_id, &local_error);
    if (local_error == NO_ERROR) {
        *error = FILE_ALREADY_EXISTS;
        return;
    }

    i_node_block_info_t i_node_block_info = get_i_node_block_info(dir_i_node_data);

    uint8_t next_item;
    uint8_t current_block_num;

    if (i_node_block_info.last_block_size != 32) {
        next_item = i_node_block_info.last_block_size / 8;
        current_block_num = i_node_block_info.number_of_blocks - 1;
    } else {
        dir_i_node_data->block_ids[i_node_block_info.number_of_blocks] = alloc_block(error);
        if (*error != NO_ERROR) {
            dir_i_node_data->block_ids[i_node_block_info.number_of_blocks] = 0;
            return;
        }

        current_block_num = i_node_block_info.number_of_blocks;
        next_item = 0;
    }

    directory_data_t dir_data = dir_data_get_by_id(dir_i_node_data->block_ids[current_block_num], error);
    if (*error != NO_ERROR) {
        return;
    }

    directory_item_t dir_item;
    dir_item.i_node_id = file_node_id;
    strcpy(dir_item.name, filename);
    dir_data.dir_items[next_item] = dir_item;
    write_bock_data_by_id(dir_i_node_data->block_ids[current_block_num], (block_data_t *) &dir_data, error);
    if (*error != NO_ERROR) {
        return;
    }

    dir_i_node_data->file_size += 8;
}

void dir_data_id__register_file_with_name(uint8_t dir_i_node_id, char* filename, uint8_t file_node_id, int *error) {
    i_node_data_t dir_i_node_data = i_node_id__get_data(dir_i_node_id, error);
    if (*error != NO_ERROR) {
        return;
    }

    dir_data__register_file_with_name(&dir_i_node_data, filename, file_node_id, error);
    if (*error == NO_ERROR) {
        i_node__save(dir_i_node_id, &dir_i_node_data, error);
        if (*error != NO_ERROR) {
            return;
        }
    }
}


void dir_data__unregister_file(i_node_data_t *dir_i_node_data, char* filename, uint8_t file_node_id, int *error) {
    *error = NO_ERROR;

    uint8_t file_folder_sector_num = 0;
    uint8_t file_postition_in_sector = 0;
    uint8_t found = 0;

//    i_node_data_t i_node_data = i_node_id__get_data(dir_i_node_id);

    for (uint8_t i = 0; i < 6 && found == 0 && i * 4 * 8 < dir_i_node_data->file_size; ++i) {
        directory_data_t dir_data = dir_data_get_by_id(dir_i_node_data->block_ids[i], error);
        if (*error != NO_ERROR) {
            return;
        }

        for (uint8_t j = 0; j < 4 && i * 4 * 8 + j * 8 < dir_i_node_data->file_size; ++j) {
            if (strcmp(dir_data.dir_items[j].name, filename) == 0 || dir_data.dir_items[j].i_node_id == file_node_id) {
                file_folder_sector_num = i;
                file_postition_in_sector = j;
                found = 1;
                break;
            }
        }
    }

    if (!found) {
        *error = NO_SUCH_FILE;
        return;
    }

    i_node_block_info_t i_node_block_info = get_i_node_block_info(dir_i_node_data);

    uint8_t dir_data_id = dir_i_node_data->block_ids[file_folder_sector_num];
    directory_data_t dir_data = dir_data_get_by_id(dir_data_id, error);
    if (*error != NO_ERROR) {
        return;
    }

    if (i_node_block_info.number_of_blocks - 1 == file_folder_sector_num) {
        dir_data.dir_items[file_postition_in_sector] = dir_data.dir_items[i_node_block_info.last_block_size / 8];
        if (i_node_block_info.last_block_size != 8) {
            directory_item_t dir_item;
            dir_item.i_node_id = 0;
            dir_item.name[0] = 0;
            dir_data.dir_items[i_node_block_info.last_block_size / 8] = dir_item;
        } else {
            dir_i_node_data->block_ids[i_node_block_info.number_of_blocks - 1] = 0;
            free_block(dir_data_id, error);
            if (*error != NO_ERROR) {
                return;
            }
        }
    } else {
        uint8_t latest_dir_data_id = dir_i_node_data->block_ids[i_node_block_info.number_of_blocks - 1];
        directory_data_t latest_data = dir_data_get_by_id(latest_dir_data_id, error);
        if (*error != NO_ERROR) {
            return;
        }

        dir_data.dir_items[file_postition_in_sector] = latest_data.dir_items[i_node_block_info.last_block_size / 8];
        if (i_node_block_info.last_block_size != 8) {
            directory_item_t dir_item;
            dir_item.i_node_id = 0;
            dir_item.name[0] = 0;
            latest_data.dir_items[i_node_block_info.last_block_size / 8] = dir_item;
            dir_data_save_by_id(latest_dir_data_id, &latest_data, error);
            if (*error != NO_ERROR) {
                return;
            }
        } else {
            dir_i_node_data->block_ids[i_node_block_info.number_of_blocks - 1] = 0;
            free_block(latest_dir_data_id, error);
            if (*error != NO_ERROR) {
                return;
            }
        }
    }

    dir_i_node_data->file_size -= 8;
}

void dir_data_id__unregister_file(uint8_t dir_i_node_id, char* filename, uint8_t file_node_id, int * error) {
    i_node_data_t dir_i_node_data = i_node_id__get_data(dir_i_node_id, error);
    if (*error != NO_ERROR) {
        return;
    }

    dir_data__unregister_file(&dir_i_node_data, filename, file_node_id, error);
    if (*error == NO_ERROR) {
        i_node__save(dir_i_node_id, &dir_i_node_data, error);
        if (*error != NO_ERROR) {
            return;
        }
    }
}