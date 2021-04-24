#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <minifs/high_level.h>
#include <minifs/textual.h>


void cmd_file_create(char * name, int *error) {
    minifs_core_textual__file_create(name, error);
}


void cmd_file_get(char * name, int *error) {
    int file_length;
    char * data = minifs_core_textual__file_get(name, &file_length, error);
    if (*error == NO_ERROR) {
        printf("%s\n", data);
    }
}


void cmd_file_write(char * name, char * contents, int *error) {
    minifs_core_textual__file_write(name, contents, (int) strlen(contents), error);
}


void cmd_file_delete(char * name, int *error) {
    minifs_core_textual__file_delete(name, error);
}


void cmd_file_copy(char * source, char * destination, int *error) {
    minifs_core_textual__file_copy(source, destination, error);
}


void cmd_dir_create(char * name, int *error) {
    minifs_core_textual__dir_create(name, error);
}

void cmd_dir_delete(char * name, int *error) {
    minifs_core_textual__dir_delete(name, error);
}


void cmd_dir_get(char * name, int *error) {
    uint8_t size;
    char * data = minifs_core_textual__dir_get_contents(name, (int *) &size, error);
    if (*error == NO_ERROR) {
        printf("%s\n", data);
    }
}


void execute_cmd(int args_count, char** args) {
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


int main() {
    int error;
    high_level__filesystem_create(&error);
    cmd();
    return 0;
}
