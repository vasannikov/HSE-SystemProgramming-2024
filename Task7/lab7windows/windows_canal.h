#ifndef SP7_CANAL_H
#define SP7_CANAL_H

#include <fcntl.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include <errno.h>

#include <stdio.h>

#include <unistd.h>
#include <math.h>
#include "myObjects.h"


VOID InitializeSecurityAttr(LPSECURITY_ATTRIBUTES attr, SECURITY_DESCRIPTOR * sd){
    attr->nLength = sizeof(SECURITY_ATTRIBUTES);
    attr->bInheritHandle = TRUE; // Включаем наследование дескрипторов
    InitializeSecurityDescriptor(sd, SECURITY_DESCRIPTOR_REVISION);
    attr->lpSecurityDescriptor = sd;
}

int start_and_wait_processes(int N, char **array_part_str){
    int i;
    HANDLE* parent_write_pipe_in = malloc(N * sizeof(HANDLE));
    HANDLE* child_read_pipe_in = malloc(N * sizeof(HANDLE));
    SECURITY_ATTRIBUTES* attr_in = malloc(N * sizeof(SECURITY_ATTRIBUTES));
    SECURITY_DESCRIPTOR* sd_in  = malloc(N * sizeof(SECURITY_DESCRIPTOR));

    HANDLE* parent_read_pipe_out = malloc(N * sizeof(HANDLE));
    HANDLE* child_write_pipe_out = malloc(N * sizeof(HANDLE));
    SECURITY_ATTRIBUTES* attr_out = malloc(N * sizeof(SECURITY_ATTRIBUTES));
    SECURITY_DESCRIPTOR* sd_out  = malloc(N * sizeof(SECURITY_DESCRIPTOR));

    HANDLE StdIn = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE StdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    PROCESS_INFORMATION pi[N];
    STARTUPINFO si[N];
    HANDLE hProcess[N];
    for (i = 0; i < N; i++) {
        int max_len_str = strlen(array_part_str[i]) + 1;
        InitializeSecurityAttr(&attr_in[i], &sd_in[i]);
        if (!CreatePipe(&child_read_pipe_in[i], &parent_write_pipe_in[i], &attr_in[i], 0)) {
            console_error();
            ExitProcess(1);
        }
        SetStdHandle(STD_INPUT_HANDLE, child_read_pipe_in[i]);

        DWORD dwBytesWritten = 0;
        WriteFile(parent_write_pipe_in[i], array_part_str[i], max_len_str, &dwBytesWritten, NULL);
        InitializeSecurityAttr(&attr_out[i], &sd_out[i]);
        if (!CreatePipe(&parent_read_pipe_out[i], &child_write_pipe_out[i], &attr_out[i], 0)) {
            console_error();
            ExitProcess(1);
        }
        SetStdHandle(STD_OUTPUT_HANDLE, child_write_pipe_out[i]);
        DWORD commandLine[MAX_FILE_NAME_LEN + strlen("subproc.exe")];
        sprintf(commandLine, "subproc.exe 0 %d 0", max_len_str);

        GetStartupInfo(&si[i]);

        BOOL res = CreateProcess(
                NULL,
                commandLine,
                NULL, NULL, TRUE, 0, NULL, NULL, &si[i], &pi[i]);
        CloseHandle(child_read_pipe_in[i]);
        CloseHandle(child_write_pipe_out[i]);
        SetStdHandle(STD_INPUT_HANDLE, StdIn);
        SetStdHandle(STD_OUTPUT_HANDLE, StdOut);
        if (!res){
            console_error();
        }
        printf("Started pid: %d\n", pi[i].dwProcessId);
        hProcess[i] = pi[i].hProcess;

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

    int sum = 0;
    char buf[10];
    for (i = 0; i < N; i++) {
        DWORD actual_len = 0;
        ReadFile(parent_read_pipe_out[i], buf, sizeof(int), &actual_len, NULL);

        sum += atoi(buf);
//        printf("Result: %d\n", sum);
    }
    printf("Result: %d\n", sum);
    return 0;
}


int main_start_processes_canal(int N, char **array_part_str){
    int status;
    status = start_and_wait_processes(N, array_part_str);
    if (status != 0){
        return 1;
    }
//    getchar();

//    int sum = get_sum_digits(N, pipes_read);
//    printf("Result: %d\n", sum);
    return EXIT_SUCCESS;
}

#endif //SP7_CANAL_H
