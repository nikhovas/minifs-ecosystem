#pragma once

#include <minifs/std-wrapper/stdint.h>
#include "i_node.h"
#include "minifs/errors.h"
#include <minifs/filesystem_context.h>


typedef struct __attribute__((packed)) {
    uint8_t i_node_id;
    char name[7];
} directory_item_t;


typedef struct __attribute__((packed)) {
    directory_item_t dir_items[4];
} directory_data_t;

// directory data loading operations
directory_data_t dir_data_get_by_id(struct minifs_core__filesystem_context * ctx, uint8_t dir_data_id, int *error);
void dir_data_save_by_id(struct minifs_core__filesystem_context * ctx, uint8_t dir_data_id, directory_data_t *block_data, int *error);


typedef struct {
    uint8_t file_i_node_id;
    uint8_t dir_i_node_id;
    uint8_t is_directory;
} found_file_info_t;

found_file_info_t dir_data__get_file_in_subdirs(
        struct minifs_core__filesystem_context * ctx,
                const i_node_data_t *dir_i_node_data,
                uint8_t dir_i_node_data_id,
                char *filepath,
                uint8_t file_i_node_id,
                int *error
                );

found_file_info_t dir_data_id__get_file_in_subdirs(
        struct minifs_core__filesystem_context * ctx,
        uint8_t dir_i_node_id,
        char *filepath,
        uint8_t file_i_node_id,
        int *error
        );

void dir_data__write_file_in_subdirs(
        struct minifs_core__filesystem_context * ctx,
                i_node_data_t *dir_i_node_data,
                uint8_t dir_i_node_data_id,
                char *filepath,
                uint8_t file_i_node_id,
                int *error
                );

void dir_data_id__write_file_in_subdirs(struct minifs_core__filesystem_context * ctx, uint8_t dir_i_node_data_id, char *filepath, uint8_t file_i_node_id, int *error);
void dir_data__unregister_file_in_subdirs(struct minifs_core__filesystem_context * ctx, i_node_data_t *dir_i_node_data, uint8_t dir_i_node_data_id, char *filepath, uint8_t file_i_node_id, int *error);
void dir_data_id__unregister_file_in_subdirs(struct minifs_core__filesystem_context * ctx, uint8_t dir_i_node_data_id, char *filepath, uint8_t file_i_node_id, int *error);


uint8_t dir_data__get_number_of_files(struct minifs_core__filesystem_context * ctx, const i_node_data_t *dir_i_node_data, int * error);
uint8_t dir_data_id__get_number_of_files(struct minifs_core__filesystem_context * ctx, uint8_t dir_i_node_id, int * error);


// some info
void dir_data__get_files_list(struct minifs_core__filesystem_context * ctx, i_node_data_t *dir_i_node_data, directory_item_t* dir_items, int * error);
void dir_data_id__get_files_list(struct minifs_core__filesystem_context * ctx, uint8_t dir_i_node_id, directory_item_t* dir_items, int * error);


// file existant in directory operations
void dir_data__register_file_with_name(struct minifs_core__filesystem_context * ctx, i_node_data_t *dir_i_node_data, char* filename, uint8_t file_node_id, int *error);
void dir_data_id__register_file_with_name(struct minifs_core__filesystem_context * ctx, uint8_t dir_i_node_id, char* filename, uint8_t file_node_id, int *error);
void dir_data__unregister_file(struct minifs_core__filesystem_context * ctx, i_node_data_t *dir_i_node_data, char* filename, uint8_t file_node_id, int *error);
void dir_data_id__unregister_file(struct minifs_core__filesystem_context * ctx, uint8_t dir_i_node_id, char* filename, uint8_t file_node_id, int *error);


// existant
uint8_t dir_data_create(struct minifs_core__filesystem_context * ctx, int *error);
void dir_data__delete(struct minifs_core__filesystem_context * ctx, i_node_data_t *dir_i_node_data, uint8_t dir_i_node_id, int *error);
void dir_data_id__delete(struct minifs_core__filesystem_context * ctx, uint8_t dir_i_node_id, int *error);


// get file i-node
uint8_t dir_data_get_file_i_node(struct minifs_core__filesystem_context * ctx, i_node_data_t *dir_i_node_data, char *filename, uint8_t file_i_node_id, int *error);


// clear
void dir_data_clear(struct minifs_core__filesystem_context * ctx, i_node_data_t *dir_i_node_data, int *error);