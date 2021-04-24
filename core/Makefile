obj-m += minifs-kernel.o
# minifs-kernel-objs := ./src/kernel.o ./src/utils.c ./src/errors.c ./src/disk.c ./src/core/bitmap.c ./src/core/block.c ./src/core/superblock.c
minifs-kernel-objs := ./src/kernel.o \
./src/middle_level/file.o \
./src/middle_level/directory.o \
./src/utils.o \
./src/errors.o \
./src/core/bitmap.o \
./src/core/block.o \
./src/core/superblock.o \
./src/core/file/existant.o \
./src/core/file/get_contents.o \
./src/core/file/existant.o \
./src/core/file/get_length.o \
./src/core/file/manage_block_data.o \
./src/core/file/overwrite.o \
./src/core/i_node/existance.o \
./src/core/i_node/existance.o \
./src/core/i_node/get_block_info.o \
./src/core/i_node/manage_block_count.o \
./src/core/directory/clear.o \
./src/core/directory/existant.o \
./src/core/directory/get_file_i_node.o \
./src/core/directory/get_files_list.o \
./src/core/directory/get_number_of_files.o \
./src/core/directory/manage_file_in_subdirs.o \
./src/core/directory/register_file.o \
./src/kernel_disk.o \
./src/transfer-protocol/create_request.o \
./src/transfer-protocol/parse_request.o \
./src/transfer-protocol/response.o


ccflags-y += -I$(PWD)/include $(call cc-option,-ffreestanding) -D KERNEL_MODULE -std=gnu99

all:
	make -C /lib/modules/5.8.0-50-generic/build M=$(PWD) modules

clean:
	make -C /lib/modules/5.8.0-50-generic/build M=$(PWD) clean