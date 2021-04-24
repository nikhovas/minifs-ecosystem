#pragma once
#include "i_node.h"
#include "block.h"
#include "minifs/errors.h"


// manage block data
block_data_t file__get_block_data(struct minifs_core__filesystem_context * ctx, i_node_data_t *i_node_data, uint8_t block_number, int *error);
void file__set_block_data(struct minifs_core__filesystem_context * ctx, i_node_data_t *i_node_data, uint8_t block_number, block_data_t* block_data, int *error);


// get contents
void file__get_contents(struct minifs_core__filesystem_context * ctx, i_node_data_t *i_node_data, uint8_t* contents, int *error);
void file_id__get_contents(struct minifs_core__filesystem_context * ctx, uint8_t i_node_id, uint8_t* contents, int *error);


// get length
uint16_t file__get_length(struct minifs_core__filesystem_context * ctx, i_node_data_t *i_node_data);
uint16_t file_id__get_length(struct minifs_core__filesystem_context * ctx, uint8_t i_node_id, int *error);


// overwrites
void file__overwrite(struct minifs_core__filesystem_context * ctx, i_node_data_t *i_node_data, uint8_t* data, uint16_t length, int * error);
void file_id__overwrite(struct minifs_core__filesystem_context * ctx, uint8_t i_node_id, uint8_t* data, uint16_t length, int * error);


// existant operations
void file_id__remove(struct minifs_core__filesystem_context * ctx, uint8_t i_node_id, int *error);
uint8_t file__create(struct minifs_core__filesystem_context * ctx, int * error);