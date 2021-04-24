#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <minifs/high_level.h>
#include <minifs/textual.h>
#include <minifs/storage-drivers/file_storage_driver.h>
#include <minifs/storage-drivers/in_memory_storage_driver.h>
#include <minifs/transfer-protocol.h>
#include <minifs/std-wrapper/memory.h>
#include <minifs/std-wrapper/string.h>


struct minifs_core__filesystem_context ctx;
//struct file_storage_driver sdi;
struct in_memory_storage_driver imsd;
void (*execute_cmd)(int args_count, char** args);

struct minifs_core_transfer_protocol_options minifs_po = {&memcpy, &memcpy, &memcpy, &memcpy};


void cmd_file_create(char * name, int *error) {
    minifs_core_textual__file_create(&ctx, name, error);
}


void cmd_file_get(char * name, int *error) {
    int file_length;
    char * data = minifs_core_textual__file_get(&ctx, name, &file_length, error);
    if (*error == NO_ERROR) {
        printf("%s\n", data);
    }
}


void cmd_file_write(char * name, char * contents, int *error) {
    minifs_core_textual__file_write(&ctx, name, contents, (int) strlen(contents), error);
}


void cmd_file_delete(char * name, int *error) {
    minifs_core_textual__file_delete(&ctx, name, error);
}


void cmd_file_copy(char * source, char * destination, int *error) {
    minifs_core_textual__file_copy(&ctx, source, destination, error);
}


void cmd_dir_create(char * name, int *error) {
    minifs_core_textual__dir_create(&ctx, name, error);
}

void cmd_dir_delete(char * name, int *error) {
    minifs_core_textual__dir_delete(&ctx, name, error);
}


void cmd_dir_get(char * name, int *error) {
    uint8_t size;
    char * data = minifs_core_textual__dir_get_contents(&ctx, name, (int *) &size, error);
    if (*error == NO_ERROR) {
        printf("%s\n", data);
    }
}


void embedded_execute_cmd(int args_count, char** args) {
    int error = 0;
    if (args_count >= 3) {
        if (strcmp(args[0], "file") == 0) {
            if (strcmp(args[1], "create") == 0) {
                cmd_file_create(args[2], &error);
            } else if (strcmp(args[1], "get") == 0) {
                cmd_file_get(args[2], &error);
            } else if (strcmp(args[1], "write") == 0) {
                if (args_count >= 4) {
                    cmd_file_write(args[2], args[3], &error);
                } else {
                    printf("Wrong cmd\n");
                }
            } else if (strcmp(args[1], "delete") == 0) {
                cmd_file_delete(args[2], &error);
            } else if (strcmp(args[1], "copy") == 0) {
                if (args_count >= 4) {
                    cmd_file_copy(args[2], args[3], &error);
                } else {
                    printf("Wrong cmd\n");
                }
            } else {
                printf("Wrong cmd\n");
            }
        } else if (strcmp(args[0], "dir") == 0) {
            if (strcmp(args[1], "create") == 0) {
                cmd_dir_create(args[2], &error);
            } else if (strcmp(args[1], "get") == 0) {
                cmd_dir_get(args[2], &error);
            } else if (strcmp(args[1], "delete") == 0) {
                cmd_dir_delete(args[2], &error);
            } else {
                printf("Wrong cmd\n");
            }
        }
    } else {
        printf("Wrong cmd\n");
    }

    switch (error) {
        case NO_ERROR:
            break;
        case NO_SUCH_FILE:
            printf("No such file/dir\n");
            break;
        case INVALID_PATH:
            printf("Invalid path\n");
            break;
        case FILE_ALREADY_EXISTS:
            printf("File already exists\n");
            break;
        case NO_FREE_NODES_AVAILABLE:
            printf("No free nodes available\n");
            break;
        case NOT_A_DIRECTORY:
            printf("Not a directory\n");
            break;
        case NOT_A_VALID_PATH:
            printf("Not a valid path\n");
            break;
        case WRONG_FILE_NAME:
            printf("Wrong file name\n");
            break;
        case FILE_ERROR:
            printf("Error while connection to disk. Your data can be damaged.\n");
            break;
        case NO_SUCH_SOURCE:
            printf("No such source.\n");
            break;
        case NO_SUCH_DESTINATION:
            printf("No such destination.\n");
            break;
        case FILE_LIMIT:
            printf("File limit.\n");
            break;
        case BLOCKS_LIMIT:
            printf("Blocks for data limit.\n");
            break;
        case TOO_BIG_FILE:
            printf("Too big file\n");
            break;
        default:
            printf("Unknown error\n");
            break;
    }

    if (error == FILE_ERROR) {
        printf("Disk error. Data can be damaged.\n");
    }
}


void kernel_execute_cmd(int args_count, char** args) {
    int error = 0;
    uint8_t buffer[1024];
    uint16_t buffer_size = 0;
    if (args_count >= 3) {
        if (strcmp(args[0], "file") == 0) {
            if (strcmp(args[1], "create") == 0) {
                buffer_size = minifs_core_protocol__file_create(&minifs_po, args[2], buffer);
            } else if (strcmp(args[1], "get") == 0) {
                buffer_size = minifs_core_protocol__file_get(&minifs_po, args[2], buffer);
            } else if (strcmp(args[1], "write") == 0) {
                if (args_count >= 4) {
                    buffer_size = minifs_core_protocol__file_write(&minifs_po, args[2], args[3], strlen(args[3]), buffer);
                } else {
                    printf("Wrong cmd\n");
                }
            } else if (strcmp(args[1], "delete") == 0) {
                buffer_size = minifs_core_protocol__file_delete(&minifs_po, args[2], buffer);
            } else if (strcmp(args[1], "copy") == 0) {
                if (args_count >= 4) {
                    buffer_size = minifs_core_protocol__file_copy(&minifs_po, args[2], args[3], buffer);
                } else {
                    printf("Wrong cmd\n");
                }
            } else {
                printf("Wrong cmd\n");
            }
        } else if (strcmp(args[0], "dir") == 0) {
            if (strcmp(args[1], "create") == 0) {
                buffer_size = minifs_core_protocol__dir_create(&minifs_po, args[2], buffer);
            } else if (strcmp(args[1], "get") == 0) {
                buffer_size = minifs_core_protocol__dir_get_contents(&minifs_po, args[2], buffer);
            } else if (strcmp(args[1], "delete") == 0) {
                buffer_size = minifs_core_protocol__dir_delete(&minifs_po, args[2], buffer);
            } else {
                printf("Wrong cmd\n");
            }
        }
    } else {
        printf("Wrong cmd\n");
    }

    uint8_t first_buffer[128];
    uint8_t first_buffer_size;
    uint8_t second_buffer[128];
    uint8_t second_buffer_size;
    uint8_t data_buffer[1024];
    uint16_t data_buffer_size;
    uint8_t operation_type;

    ssize_t res = minifs_core_protocol__parse_request(&minifs_po, buffer, buffer_size, first_buffer, &first_buffer_size,
                                                      second_buffer, &second_buffer_size, data_buffer, &data_buffer_size,
                                                      &operation_type);

    switch (error) {
        case NO_ERROR:
            break;
        case NO_SUCH_FILE:
            printf("No such file/dir\n");
            break;
        case INVALID_PATH:
            printf("Invalid path\n");
            break;
        case FILE_ALREADY_EXISTS:
            printf("File already exists\n");
            break;
        case NO_FREE_NODES_AVAILABLE:
            printf("No free nodes available\n");
            break;
        case NOT_A_DIRECTORY:
            printf("Not a directory\n");
            break;
        case NOT_A_VALID_PATH:
            printf("Not a valid path\n");
            break;
        case WRONG_FILE_NAME:
            printf("Wrong file name\n");
            break;
        case FILE_ERROR:
            printf("Error while connection to disk. Your data can be damaged.\n");
            break;
        case NO_SUCH_SOURCE:
            printf("No such source.\n");
            break;
        case NO_SUCH_DESTINATION:
            printf("No such destination.\n");
            break;
        case FILE_LIMIT:
            printf("File limit.\n");
            break;
        case BLOCKS_LIMIT:
            printf("Blocks for data limit.\n");
            break;
        case TOO_BIG_FILE:
            printf("Too big file\n");
            break;
        default:
            printf("Unknown error\n");
            break;
    }

    if (error == FILE_ERROR) {
        printf("Disk error. Data can be damaged.\n");
    }
}


void cmd() {
    while (1) {
        char data[1024];
        int args_count = 0;
        char *args[32];
        fgets(data, 1024, stdin);
        data[strlen(data) - 1] = 0;
        if (strcmp(data, "exit") == 0) {
            break;
        }
        const char delim[] = " ";

        for (char *ptr = strtok(data, delim); ptr != NULL; ptr = strtok(NULL, delim)) {
            args[args_count++] = ptr;
        }

        execute_cmd(args_count, args);
    }
}


void embedded_filesystem() {
    int error;

    minifs_core__create_in_memory_storage_driver(&imsd);
    ctx.sdi = (struct storage_driver_interface *) &imsd;
    imsd.init(&ctx, &error);
    cmd();
}


void kernel_filesystem(const char * symbol_device_path) {
    cmd();
}


int main(int argc, char* argv[]) {
    if (argc == 3) {
        if (strcmp(argv[1], "kernel") == 0) {
            execute_cmd = &kernel_execute_cmd;
            kernel_filesystem(argv[2]);
        } else {
            execute_cmd = &embedded_execute_cmd;
            embedded_filesystem();
        }
    } else {
        execute_cmd = &embedded_execute_cmd;
        embedded_filesystem();
    }

    return 0;
}
