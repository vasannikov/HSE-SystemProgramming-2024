#ifndef SP5_MYOBJECTS_H
#define SP5_MYOBJECTS_H

#define MAX_LEN_IN_FILE 64
#define MAX_FILE_NAME_LEN 64
#define MAX_STR_LEN 10
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

void console_error(){
    LPSTR error_message;
    DWORD errorMessageLength = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,NULL,GetLastError(),MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),(LPSTR)&error_message,0,NULL);
    WriteFile(GetStdHandle(STD_ERROR_HANDLE), error_message, errorMessageLength, &errorMessageLength, NULL);
    LocalFree(error_message);
}

#endif //SP5_MYOBJECTS_H
