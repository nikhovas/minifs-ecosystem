add_library(minifs-core
    src/core/directory/clear.c
    src/core/directory/existant.c
    src/core/directory/get_file_i_node.c
    src/core/directory/get_files_list.c
    src/core/directory/get_number_of_files.c
    src/core/directory/manage_file_in_subdirs.c
    src/core/directory/register_file.c
    src/core/file/existant.c
    src/core/file/get_contents.c
    src/core/file/get_length.c
    src/core/file/manage_block_data.c
    src/core/file/overwrite.c
    src/core/i_node/existance.c
    src/core/i_node/get_block_info.c
    src/core/i_node/manage_block_count.c
    src/core/bitmap.c
    src/core/block.c
    src/core/superblock.c
    src/high_level/directory.c
    src/high_level/file.c
        src/utils.c
        src/textual.c
        src/storage-drivers/file_storage_driver.c
        src/storage-drivers/in_memory_storage_driver.c
        src/transfer-protocol/create_request.c
        src/transfer-protocol/parse_request.c
        src/transfer-protocol/response.c
)


INCLUDE_DIRECTORIES(minifs-core include)

SET(MINIFS_CORE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)