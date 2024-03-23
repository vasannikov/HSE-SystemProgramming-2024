#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <windows.h>
#include "myObjects.h"




int canal(int max_len_str){
    char *buffer = (char*)malloc(sizeof(char)*(max_len_str));;
    DWORD dwBytesWritten = 0;
    ReadFile(GetStdHandle(STD_INPUT_HANDLE), buffer, (DWORD)max_len_str, &dwBytesWritten, NULL);
    int intCount = 0;
    for (int i = 0; i < strlen(buffer); i++) {
//        printf("%c", buffer[i]);
        if (islower(buffer[i])) {
            intCount++;
        }
    }
    char res[MAX_STR_LEN];
    sprintf(res, "%d\0", intCount);
    WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), res, MAX_STR_LEN, &dwBytesWritten, NULL);
    ExitProcess(0);
}


int shm(int len_str, int offset_str){
    HANDLE hMapping = OpenFileMappingA(FILE_MAP_READ , FALSE, "/data");
    char * memory = MapViewOfFile (hMapping, FILE_MAP_READ , 0, 0, MAX_BUF_SHM);

    if (memory == NULL) {
        console_error();
        ExitProcess(1);
    }
    int i;
    int intCount = 0;
    for (i = offset_str; i < offset_str + len_str; i++) {
        if (islower(memory[i])) {
            intCount++;
        }
    }
    printf("res: %d\n", intCount);
    if (!UnmapViewOfFile(memory)) {
        console_error();
        ExitProcess(1);
    }
    CloseHandle(memory);

    HANDLE hMutex = OpenMutex(SYNCHRONIZE, FALSE, "/mutex");
    WaitForSingleObject(hMutex, INFINITE);
    hMapping = OpenFileMappingA(FILE_MAP_ALL_ACCESS , FALSE, "/return");
    memory = MapViewOfFile (hMapping, FILE_MAP_ALL_ACCESS , 0, 0, MAX_BUF_SHM);

    if (memory == NULL) {
        console_error();
        ExitProcess(1);
    }
    *memory += intCount;
    ReleaseMutex(hMutex);

    if (!UnmapViewOfFile(memory)) {
        console_error();
        ExitProcess(1);
    }
    CloseHandle(memory);
    ExitProcess(0);
}


int main(int argc, char *argv[]) {
    Sleep(10000);
    if (argc != 4) {
//        printf("Usage: ./child <flag canal or shm> <len_str> <offset str>\n");
        ExitProcess(1);
    }
    int flag = atoi(argv[1]);
    int max_len_str = atoi(argv[2]);
    int offset_str = atoi(argv[3]);
    if (flag == 0){
        canal(max_len_str);
    } else {
        shm(max_len_str, offset_str);
    }
//    Sleep(max_len_str * 10);
}
