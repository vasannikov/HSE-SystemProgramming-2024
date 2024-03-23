#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include <stdio.h>
#include "windows_canal.h"
#include "windows_shm.h"


char get_array(int N, char *buffer, char **array_part_str){
    int len = strlen(buffer);
    int average_part_len = len / N;
    int start_last_part = average_part_len * (N - 1);
    int i, j;
    for (i = 0; i < N - 1; i++) {
        array_part_str[i] = malloc(average_part_len * sizeof(char *) + sizeof('\0'));
    }

    for (i = 0; i < N - 1; i++) {
        for (j = i * average_part_len; j < len; j++) {
            if ((j - i * average_part_len) % average_part_len == 0 && (j - i * average_part_len) != 0) {
                break;
            }
            array_part_str[i][(j - i * average_part_len) % average_part_len] = buffer[j];
        }
        array_part_str[i][average_part_len] = '\0';
        printf("Part of data: %s len: %ld\n", array_part_str[i], strlen(array_part_str[i]));
    }
    array_part_str[N-1] = malloc((len - start_last_part) * sizeof(char *) + sizeof('\0'));
    for (j = start_last_part; j < len; j++) {
        array_part_str[N - 1][j - start_last_part] = buffer[j];
    }
    array_part_str[N-1][len - start_last_part] = '\0';
    printf("Last part of data: %s len: %ld\n", array_part_str[N-1], strlen(array_part_str[N-1]));
    return EXIT_SUCCESS;
}

void array_free_memory(int N, char **array_part_str) {
    int i;
    for (i = 0; i < N; i++) {
        free(array_part_str[i]);
    }
    free(array_part_str);
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: ./program <name file> <N process>\n");
        return 1;
    }

    int N = atoi(argv[2]);
    if (N <= 0) {
        printf("Incorrect N");
        return 1;
    }

    LPSTR buffer = malloc(MAX_LEN_IN_FILE);
    if (buffer == NULL) {
        print_error();
        return 1;
    }
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        perror(argv[1]);
        free(buffer);
        return 1;
    }
    if (getc(file) == EOF) {
        printf("File is empty");
        free(buffer);
        fclose(file);
        return 1;
    }
    fseek(file,0,0);
    fscanf(file, "%s", buffer);
    if ((strlen(buffer) > 0) & (strlen(buffer) < 2)){
        printf("In file less than 2 char");
        free(buffer);
        fclose(file);
        return 1;
    }
    fclose(file);

    int len = strlen(buffer);
    if (N > len/2){
        N = len / 2;
        printf("Reducing number of processes\n");
    }

    char **array_part_str = malloc(N * sizeof(char *));
    int status;
    get_array(N, buffer, array_part_str);

    int flag_choose;
    printf("Choost 0-canal, 1-shm: ");
    scanf("%d", &flag_choose);
    if (flag_choose == 0){
        status = main_start_processes_canal(N, array_part_str);
    } else {
        status = main_start_processes_shm(N, array_part_str, buffer);
    }
    free(buffer);
    array_free_memory(N, array_part_str);
    if (status != 0){
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
