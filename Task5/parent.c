#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define MAX_FILE_SIZE 10000 // Максимальный размер файла

void printErrorMessageAndExit(const char *errorMessage, DWORD errorCode)
{
    fprintf(stderr, "%s Error code: %lu\n", errorMessage, errorCode);
    ExitProcess(1);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <input_file> <N>\n", argv[0]);
        ExitProcess(1);
    }

    const char *inputFileName = argv[1];
    int N = atoi(argv[2]);

    FILE *inputFile = fopen(inputFileName, "r");
    if (inputFile == NULL)
    {
        printErrorMessageAndExit("Error opening input file.", GetLastError());
    }

    fseek(inputFile, 0, SEEK_END);
    long fileSize = ftell(inputFile);
    fseek(inputFile, 0, SEEK_SET);

    if (fileSize < 2)
    {
        printErrorMessageAndExit("Input file must contain at least 2 characters.", 0);
    }

    char inputString[MAX_FILE_SIZE]; // Максимальная длина файла
    fread(inputString, sizeof(char), fileSize, inputFile);
    fclose(inputFile);

    int numberOfProcesses = N;
    int M = fileSize;

    if (N > M / 2)
    {
        numberOfProcesses = M / 2;
        printf("Reducing the number of child processes to %d\n", numberOfProcesses);
    }

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    char command[100];

    int segmentSize = M / numberOfProcesses;
    int extraCharsLastProcess = M - (numberOfProcesses - 1) * (M / numberOfProcesses);
    int totalLowercaseCount = 0;

    int startPos = 0;
    int currentProcess = 0;

    while (startPos < M)
    {
        int segmentLength = (currentProcess == numberOfProcesses - 1) ? extraCharsLastProcess : segmentSize;

        printf("Creating child process %d for segment: ", currentProcess);
        for (int j = startPos; j < startPos + segmentLength; j++)
        {
            putchar(inputString[j]);
        }
        putchar('\n');

        sprintf(command, "child.exe %s %d %d %d", inputFileName, startPos, segmentLength, currentProcess);
        if (!CreateProcess(NULL, command, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
        {
            printErrorMessageAndExit("Error creating child process.", GetLastError());
        }

        WaitForSingleObject(pi.hProcess, INFINITE);

        DWORD exitCode;
        GetExitCodeProcess(pi.hProcess, &exitCode);
        totalLowercaseCount += exitCode;

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        startPos += segmentLength;
        currentProcess++;
    }

    printf("Total lowercase count from all processes: %d\n", totalLowercaseCount);

    ExitProcess(0);
}
