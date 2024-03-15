#include <stdio.h>
#include <windows.h>

void printErrorMessageAndExit(const char *errorMessage, DWORD errorCode)
{
    fprintf(stderr, "%s Error code: %lu\n", errorMessage, errorCode);
    ExitProcess(1);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <child_process>\n", argv[0]);
        ExitProcess(1);
    }

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    char command[100];

    sprintf(command, "%s", argv[1]);
    if (!CreateProcess(NULL, command, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        printErrorMessageAndExit("Error creating child process.", GetLastError());
    }

    // Не дожидаемся завершения дочернего процесса
    Sleep(500);

    ExitProcess(0);
}
