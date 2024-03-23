#ifndef SP7_SHM_H
#define SP7_SHM_H

#include <fcntl.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include <errno.h>

#include <stdio.h>

#include <unistd.h>
#include <math.h>
#include "myObjects.h"

int start_and_wait_processes_shm(int N, char **array_part_str){
    int i;
    int offset = 0;
    PROCESS_INFORMATION pi[N];
    STARTUPINFO si[N];
    HANDLE hProcess[N];
    for (i = 0; i < N; i++) {
        int max_len_str = strlen(array_part_str[i]) + 1;

        DWORD commandLine[MAX_FILE_NAME_LEN + strlen("subproc.exe")];
        sprintf(commandLine, "subproc.exe 1 %d %d", max_len_str, offset);

        GetStartupInfo(&si[i]);

        BOOL res = CreateProcess(
                NULL,
                commandLine,
                NULL, NULL, TRUE, 0, NULL, NULL, &si[i], &pi[i]);

        if (!res){
            console_error();
        }
        printf("Started pid: %d\n", pi[i].dwProcessId);
        hProcess[i] = pi[i].hProcess;
        offset += max_len_str;
        //Sleep(30000);
    }

//    getchar();

    DWORD dwRes = WaitForMultipleObjects(N, hProcess, TRUE, INFINITE);
    if (dwRes == WAIT_FAILED){
        console_error();
        return 1;
    }
    BOOL error_flag = 0;
    for (i = 0; i < N; i++){
        int code;
        GetExitCodeProcess(hProcess[i], &code);
        printf("Process with pid %d exited with code %d\n", pi[i].dwProcessId, code);
        if (code != 0){
            error_flag = 1;
        }
    }
    if (error_flag == 1){
        return 1;
    }
    printf ("Child process exited \n");
    return 0;
}


int write_shm_func(char *name_shm, void *variable, int max_buf){
//    void *memory = MAP_FAILED;
    HANDLE hMapping = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,
                                         0, max_buf, name_shm);
    char * memory = MapViewOfFile(hMapping,
                               FILE_MAP_ALL_ACCESS,
                               0, 0, max_buf);
    if (memory == NULL) {
        console_error();
        ExitProcess(1);
    }
    memcpy(memory, variable, max_buf);

    if (!UnmapViewOfFile(memory)) {
        console_error();
        ExitProcess(1);
    }
    CloseHandle(memory);
    return 0;
}

int check_result_shm(){
    char name_shm[] = "/return";
    HANDLE hMapping = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,
                                         0, sizeof(int), name_shm);
    char * memory = MapViewOfFile(hMapping,
                               FILE_MAP_ALL_ACCESS,
                               0, 0, sizeof(int));
    printf("Result: %d\n", *memory);

    if (!UnmapViewOfFile(memory)) {
        console_error();
        ExitProcess(1);
    }
    CloseHandle(memory);
    return EXIT_SUCCESS;
}

HANDLE hMutex;
int main_start_processes_shm(int N, char **array_part_str, char *buffer){
    int status_shm;
    status_shm = write_shm_func("/data", buffer, strlen(buffer));
    if (status_shm != 0) {
        return 1;
    }

    int value = 0;
    int *result = &value;
    status_shm = write_shm_func("/return", result, sizeof(int));
    if (status_shm != 0) {
        return 1;
    }
    hMutex = CreateMutex(NULL, FALSE, "/pr7mutex");
//    getchar();

    status_shm = start_and_wait_processes_shm(N, array_part_str);
    if (status_shm != 0){
        return 1;
    }
    check_result_shm();
    CloseHandle(hMutex);
    return 0;
}

#endif //SP7_SHM_H
