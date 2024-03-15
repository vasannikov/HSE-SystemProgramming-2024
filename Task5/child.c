#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <ctype.h>

#define MAX_SEGMENT_SIZE 256 // Максимальный размер сегмента

void printErrorMessageAndExit(const char *errorMessage, DWORD errorCode)
{
    fprintf(stderr, "%s Error code: %lu\n", errorMessage, errorCode);
    exit(1);
}

int countLowercase(const char *segment, int segmentLength)
{
    int lowercaseCount = 0;
    for (int i = 0; i < segmentLength; i++)
    {
        if (islower(segment[i]))
        {
            lowercaseCount++;
        }
    }
    return lowercaseCount;
}

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        fprintf(stderr, "Usage: %s <input_file> <start_pos> <segment_length> <process_number>\n", argv[0]);
        return 1;
    }

    const char *inputFileName = argv[1];
    int startPos = atoi(argv[2]);
    int segmentLength = atoi(argv[3]);
    int processNumber = atoi(argv[4]);

    FILE *inputFile = fopen(inputFileName, "r");
    if (inputFile == NULL)
    {printErrorMessageAndExit("Error opening input file.", GetLastError());
    }

    fseek(inputFile, startPos, SEEK_SET);

    char segment[MAX_SEGMENT_SIZE];
    fread(segment, sizeof(char), segmentLength, inputFile);
    fclose(inputFile);

    int lowercaseCount = countLowercase(segment, segmentLength);

    printf("Child process %d completed with lowercase count: %d\n", processNumber, lowercaseCount);

    return lowercaseCount;
}