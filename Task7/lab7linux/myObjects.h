#ifndef SP5_MYOBJECTS_H
#define SP5_MYOBJECTS_H

#define MAX_LEN_IN_FILE 64
#define MAX_STR_LEN 10
#define MAX_RES_FILE_NAME_LEN 16
#define MAX_BUF_SHM 1024
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

void print_error() {
    char error_str[MAX_STR_LEN];
    sprintf(error_str, "Code: %d", errno);
    perror(error_str);
}

#endif //SP5_MYOBJECTS_H
