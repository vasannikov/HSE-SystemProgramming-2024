#ifndef SP7_SHM_H
#define SP7_SHM_H

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <stdio.h>

#include <unistd.h>
#include <math.h>
#include <wait.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <pthread.h>
#include "myObjects.h"

int start_processes_shm(int N, char **array_part_str, int pipes_read[N], char *buffer){
    int i;
    int offset = 0;

    for (i = 0; i < N; i++) {
        int max_len_str = strlen(array_part_str[i]);
        pid_t res = fork();
        switch(res)
        {
            case 0:
            {
                char len_proc[MAX_STR_LEN];
                sprintf(len_proc, "%d", max_len_str);
                char offset_proc[MAX_STR_LEN];
                sprintf(offset_proc, "%d", offset);
                char * args[5] = {"subproc", "1", len_proc, offset_proc, NULL};
                execve("./subproc", args, NULL);
            }
            case -1:
            {
                perror("Error after calling fork");
                return EXIT_FAILURE;
            }

        }
//        sleep(1);
        offset += max_len_str;
    }
    return EXIT_SUCCESS;
}


int wait_processes_shm(int N){
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


int clear_memory(void *memory, int fd, char *name_shm, int max_buf) {
    if (munmap(memory, max_buf) == -1){
        close(fd);
        shm_unlink(name_shm);
        print_error();
        return EXIT_FAILURE;
    }
    close(fd);
    return EXIT_SUCCESS;
}

int write_shm_func(char *name_shm, void *variable, int max_buf, int flag_mutex){
    void *memory = MAP_FAILED;
    int fd = -1;
    //    shm_unlink(name_shm);
    fd = shm_open(name_shm, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1){
        print_error();
        return EXIT_FAILURE;
    }
    if (ftruncate(fd, MAX_BUF_SHM) == -1) {
        shm_unlink(name_shm);
        close(fd);
        print_error();
        return EXIT_FAILURE;
    }
    memory = mmap(NULL, max_buf, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (memory == MAP_FAILED){
        close(fd);
        shm_unlink(name_shm);
        print_error();
        return EXIT_FAILURE;
    }

    if (flag_mutex == 1) {
//        pthread_mutex_t *memory = MAP_FAILED;
        pthread_mutexattr_t mta;
        pthread_mutexattr_init(&mta);
        pthread_mutexattr_setpshared(&mta, PTHREAD_PROCESS_SHARED);
        pthread_mutex_init(memory, &mta);
    } else {
//        char *memory = MAP_FAILED;
        memcpy(memory, variable, max_buf);
//        printf("memory: %s\n", memory);
    clear_memory(memory, fd, name_shm, max_buf);
    }
    return EXIT_SUCCESS;
}

int destroy_mutex_func(){
    char name_shm[] = "/pr7mutex";
    int fd;
    fd = -1;
    fd = shm_open(name_shm, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1){
        print_error();
        return EXIT_FAILURE;
    }
    pthread_mutex_t *mutex_memory = MAP_FAILED;
    mutex_memory = mmap(NULL, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mutex_memory == MAP_FAILED){
        close(fd);
        shm_unlink(name_shm);
        print_error();
        return EXIT_FAILURE;
    }
    pthread_mutex_destroy(mutex_memory);
    clear_memory(mutex_memory, fd, name_shm, sizeof(pthread_mutex_t));
    return EXIT_SUCCESS;
}

int check_result(){
    char name_shm[] = "/return";
    int fd;
    fd = -1;
    fd = shm_open(name_shm, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1){
        print_error();
        return EXIT_FAILURE;
    }
//    int fd = -1;
//    fd = shm_open("/return", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
//    char *memory = MAP_FAILED;
    char *memory = MAP_FAILED;
    memory = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (memory == MAP_FAILED){
//        printf("wegwerhwerh\n");
        close(fd);
        shm_unlink(name_shm);
        print_error();
        return EXIT_FAILURE;
    }
    printf("Result: %d\n", *memory);
    clear_memory(memory, fd, name_shm, sizeof(int));
    return EXIT_SUCCESS;
}

int main_start_processes_shm(int N, char **array_part_str, char *buffer){
    shm_unlink("/data");
    shm_unlink("/return");
    shm_unlink("/pr7mutex");

    int status_shm;
    status_shm = write_shm_func("/data", buffer, strlen(buffer), 0);
    if (status_shm != 0) {
        shm_unlink("/data");
        return EXIT_FAILURE;
    }

    int value = 0;
    int *result = &value;
    status_shm = write_shm_func("/return", result, sizeof(int), 0);
    if (status_shm != 0) {
        shm_unlink("/data");
        shm_unlink("/return");
        return EXIT_FAILURE;
    }
    status_shm = write_shm_func("/pr7mutex", NULL, sizeof(pthread_mutex_t), 1);
    if (status_shm != 0) {
        shm_unlink("/data");
        shm_unlink("/return");
        shm_unlink("/pr7mutex");
        return EXIT_FAILURE;
    }

    int status;
    int pipes_read[N];
    status = start_processes_shm(N, array_part_str, pipes_read, buffer);
    if (status != 0){
        shm_unlink("/data");
        shm_unlink("/return");
        shm_unlink("/pr7mutex");
        return EXIT_FAILURE;
    }

//    getchar();

    status = wait_processes_shm(N);
    shm_unlink("/data");
    if (destroy_mutex_func()){
        shm_unlink("/return");
        shm_unlink("/pr7mutex");
        return EXIT_FAILURE;
    }

    destroy_mutex_func();
    shm_unlink("/pr7mutex");

    if (status != 0) {
        return EXIT_FAILURE;
    }
    check_result();
    return EXIT_SUCCESS;
}

#endif //SP7_SHM_H
