#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>

#include <fcntl.h>
#include <ctype.h>
#include <sys/mman.h>
#include <errno.h>
#include <sys/shm.h>
#include <pthread.h>
#include "myObjects.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>

#include <fcntl.h>
#include <ctype.h>


int linux_canal(int max_len_str){
    char buffer[max_len_str];
    read(STDIN_FILENO, &buffer, max_len_str);

    int intCount = 0;
    for (int i = 0; i < strlen(buffer); i++) {
        if (islower(buffer[i])) {
            intCount++;
        }
    }
//    printf("%d", intCount);
    char res[10];
    sprintf(res, "%d", intCount);
    write(STDOUT_FILENO, res, 10);
    exit(EXIT_SUCCESS);
}

int linux_shm(int len_str, int offset_str){
    int fd = -1;
    fd = shm_open("/data", O_RDONLY, S_IRUSR);
    char * memory = MAP_FAILED;
    memory = mmap(NULL, MAX_BUF_SHM, PROT_READ, MAP_SHARED, fd, 0);
    if (memory == MAP_FAILED){
        exit(EXIT_FAILURE);
    }
    int status = munmap(NULL, MAX_BUF_SHM);

    int i;
    int intCount = 0;
    for (i = offset_str; i < offset_str + len_str; i++) {
        if (islower(memory[i])) {
            intCount++;
        }
    }
    munmap(memory, MAX_BUF_SHM);
    close(fd);
    printf("res: %d\n", intCount);
//    printf("123: %s\n", memory);


    int mfd = -1;
    mfd = shm_open("/pr7mutex", O_RDWR, S_IRUSR | S_IWUSR);
    pthread_mutex_t * mutex_memory = MAP_FAILED;
    mutex_memory = mmap(NULL, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED, mfd, 0);
    if (mutex_memory == MAP_FAILED){
        exit(EXIT_FAILURE);
    }

    pthread_mutex_lock(mutex_memory);

    fd = -1;
    fd = shm_open("/return", O_RDWR, S_IRUSR | S_IWUSR);
    memory = MAP_FAILED;
    memory = mmap(NULL, MAX_BUF_SHM, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (memory == MAP_FAILED){
        exit(EXIT_FAILURE);
    }
    *memory += intCount;
    munmap(memory, MAX_BUF_SHM);
    pthread_mutex_unlock(mutex_memory);
    munmap(mutex_memory, sizeof(pthread_mutex_t));
    close(mfd);
//    printf(": %d ", *memory);
    exit(EXIT_SUCCESS);
}


int main(int argc, char *argv[]) {
    sleep(30);
    if (argc != 4) {
//        write(STDERR_FILENO, "Usage: ./child <len_str>\n", strlen("Usage: ./child <len_str>\n"));
        exit(EXIT_FAILURE);
    }
    int flag = atoi(argv[1]);
    int max_len_str = atoi(argv[2]);
    int offset_str = atoi(argv[3]);
    if (flag == 0){
        linux_canal(max_len_str);
    } else {
        linux_shm(max_len_str, offset_str);
    }
    exit(EXIT_SUCCESS);
}
