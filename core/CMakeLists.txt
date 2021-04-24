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
    src/high_level/filesystem.c
    src/disk.c
    src/utils.c
        src/textual.c
)

INCLUDE_DIRECTORIES(minifs-core include)

SET(MINIFS_CORE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)


list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
find_package(KernelHeaders REQUIRED)
include_directories(${KERNELHEADERS_INCLUDE_DIRS})
message(status ${KERNELHEADERS_INCLUDE_DIRS})
set(DRIVER_FILE minifs-kernel.ko)
set(KBUILD_CMD $(MAKE) -C ${KERNELHEADERS_DIR} modules M=${CMAKE_CURRENT_BINARY_DIR} src=${CMAKE_CURRENT_SOURCE_DIR})

# Generate the Kbuild file through cmake.
FILE(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/Kbuild "EXTRA_CFLAGS=-I${CMAKE_CURRENT_SOURCE_DIR}/include\nobj-m := minifs-kernel.o\nminifs-kernel-y := src/minifs-kernel.c")

add_custom_command(OUTPUT ${DRIVER_FILE}
        COMMAND ${KBUILD_CMD}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS src/minifs-kernel.c VERBATIM)

add_custom_target(driver ALL DEPENDS ${DRIVER_FILE})