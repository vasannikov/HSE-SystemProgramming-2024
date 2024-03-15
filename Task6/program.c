#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include <stdbool.h>

CRITICAL_SECTION cs;
int sharedCounter = 0;

typedef struct {
    char* data;
    int start;
    int end;
} ThreadData;

void errorExit(const char* message) {
    LPSTR errorText = NULL;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&errorText, 0, NULL);
    fprintf(stderr, "%s: %s\n", message, errorText);
    LocalFree(errorText);
    ExitProcess(1);
}

DWORD WINAPI countLowerCase(void* param) {
    ThreadData* tData = (ThreadData*)param;
    int localCounter = 0;

    for (int i = tData->start; i < tData->end; i++) {
        if (islower(tData->data[i])) {
            localCounter++;
        }
    }

    EnterCriticalSection(&cs);
    sharedCounter += localCounter;
    LeaveCriticalSection(&cs);

    printf("Thread %d processed: %.*s\n", GetCurrentThreadId(), tData->end - tData->start, tData->data + tData->start);
    printf("Thread %d counted lowercase letters: %d\n", GetCurrentThreadId(), localCounter);
    Sleep(30000); // Пауза на 30 секунд

    ExitThread(0);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file> <number_of_threads>\n", argv[0]);
        ExitProcess(1);
    }

    InitializeCriticalSection(&cs);

    char* filename = argv[1];
    int numThreads = atoi(argv[2]);

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        errorExit("Error opening file");
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    if (fileSize < 2) {
        fprintf(stderr, "File should contain at least 2 characters\n");
        fclose(file);
        ExitProcess(1);
    }

    int maxThreads = fileSize / 2;
    if (numThreads > maxThreads) {
        numThreads = maxThreads;
        printf("Number of threads reduced to %d to meet the requirement.\n", numThreads);
    }

    char* fileContent = (char*)malloc(fileSize + 1);
    if (fileContent == NULL) {
        errorExit("Error allocating memory for file content");
    }

    if (fread(fileContent, 1, fileSize, file) != fileSize) {
        free(fileContent);
        fclose(file);
        errorExit("Error reading file content");
    }
    fileContent[fileSize] = '\0';

    ThreadData* threadsData = (ThreadData*)malloc(numThreads * sizeof(ThreadData));
    if (threadsData == NULL) {
        free(fileContent);
        fclose(file);
        errorExit("Error allocating memory for threads data");
    }

    HANDLE* threadHandles = (HANDLE*)malloc(numThreads * sizeof(HANDLE));
    if (threadHandles == NULL) {
        free(fileContent);
        free(threadsData);
        fclose(file);
        errorExit("Error allocating memory for thread handles");
    }

    int segmentSize = fileSize / numThreads;
    int remainingChars = fileSize % numThreads;

    int currentStart = 0;
    for (int i = 0; i < numThreads; i++) {
        threadsData[i].data = fileContent;
        threadsData[i].start = currentStart;
        threadsData[i].end = currentStart + segmentSize + (i < remainingChars ? 1 : 0);

        currentStart = threadsData[i].end;
    }

    for (int i = 0; i < numThreads; i++) {
        threadHandles[i] = CreateThread(NULL, 0, countLowerCase, &threadsData[i], 0, NULL);
        if (threadHandles[i] == NULL) {
            free(fileContent);
            free(threadsData);
            fclose(file);
            errorExit("Error creating thread");
        }
    }

    WaitForMultipleObjects(numThreads, threadHandles, TRUE, INFINITE);

    fclose(file);
    free(fileContent);
    free(threadsData);
    free(threadHandles);
    DeleteCriticalSection(&cs);

    printf("Total lowercase letters in the file: %d\n", sharedCounter);

    ExitProcess(0);
}