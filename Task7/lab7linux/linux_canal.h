#ifndef SP7_CANAL_H
#define SP7_CANAL_H

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <stdio.h>

#include <unistd.h>
#include <math.h>
#include <wait.h>
#include "myObjects.h"


int start_processes(int N, char **array_part_str, int pipes_read[N]){
    int i;
    for (i = 0; i < N; i++) {
        int max_len_str = strlen(array_part_str[i]) + 1;
        int fd_pair[2];

        if (pipe(fd_pair) < 0) {
            print_error();
            return EXIT_FAILURE;
        }
        int child_read_pipe = fd_pair[0];
        int parent_write_pipe = fd_pair[1];

        if (pipe(fd_pair) < 0) {
            print_error();
            return EXIT_FAILURE;
        }
        int child_read_pipe_return = fd_pair[0];
        int parent_write_pipe_return = fd_pair[1];

        pipes_read[i] = child_read_pipe_return;
        pid_t res = fork();
        switch(res)
        {
            case 0:
            {
                dup2(child_read_pipe, STDIN_FILENO);
                dup2(parent_write_pipe_return, STDOUT_FILENO);
                close(parent_write_pipe);
                close(child_read_pipe);
                close(parent_write_pipe_return);
                close(child_read_pipe_return);
                char len_str[MAX_STR_LEN];
                sprintf(len_str, "%d", max_len_str);
                char * args[5] = {"subproc", "0", len_str, "0", NULL};
                execve("./subproc", args, NULL);
            }
            case -1:
            {
                perror("Error after calling fork");
                return EXIT_FAILURE;
            }
            default:
                close(child_read_pipe);
                close(parent_write_pipe_return);

        }
        write(parent_write_pipe, array_part_str[i], max_len_str);
        close(parent_write_pipe);
    }
    return EXIT_SUCCESS;
}


int wait_processes(int N){
    for (int i = 0; i < N; i++) {
        int code;
        pid_t child = wait(&code);
        if (child == -1){
            perror("Error after calling wait");
            return EXIT_FAILURE;
        }
        int code_return = WEXITSTATUS(code);
        if (code_return != 0){
            return EXIT_FAILURE;
        }
        printf("Process with pid %d exited with code %d\n", child, code_return);
    }
    return EXIT_SUCCESS;
}


int get_sum_digits(int N, int pipes_read[N]) {
    int i;
    int sum = 0;
    char buf[MAX_STR_LEN];
    for (i = 0; i < N; i++) {
        read(pipes_read[i], buf, MAX_STR_LEN);
        printf("Procces %d result: %d\n", i, atoi(buf));
        sum += atoi(buf);
        close(pipes_read[i]);
    }
    return sum;
}


int main_start_processes_canal(int N, char **array_part_str){
    int status;
    int pipes_read[N];
    status = start_processes(N, array_part_str, pipes_read);
    if (status != 0){
        return EXIT_FAILURE;
    }
//    getchar();

    status = wait_processes(N);

    if (status != 0) {
        return EXIT_FAILURE;
    }
    int sum = get_sum_digits(N, pipes_read);
    printf("Result: %d\n", sum);
    return EXIT_SUCCESS;
}

#endif //SP7_CANAL_H
