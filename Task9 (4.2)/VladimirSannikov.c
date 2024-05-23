#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>

int main()
{
    // Этап 1: Инициализация работы программы и выделение памяти
    DWORD currentPID = GetCurrentProcessId();
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    char exitChar = 'q';  // Символ, по введению которого программа завершится
    char inputChar;

    // Вывод PID текущего процесса
    printf("Current Process ID: %lu\n", currentPID);

    printf("Enter '%c' to exit the program.\n", exitChar);

    while (1)
    {
        // Этап 3: Получение списка процессов
        hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hProcessSnap == INVALID_HANDLE_VALUE)
        {
            printf("Error: Unable to create tool help snapshot!");
            return 1;
        }

        pe32.dwSize = sizeof(PROCESSENTRY32);

        if (!Process32First(hProcessSnap, &pe32))
        {
            printf("Error: Unable to retrieve information about the first process.");
            CloseHandle(hProcessSnap);
            return 1;
        }

        do
        {
            printf("PID: %lu | %s", pe32.th32ProcessID, pe32.szExeFile);

            if (pe32.th32ProcessID == currentPID)
            {
                printf(" (This process)");
            }
            printf("\n");

        } while (Process32Next(hProcessSnap, &pe32));

        CloseHandle(hProcessSnap);
        
        printf("Press '%c' to exit or any other key to refresh process list: ", exitChar);
        inputChar = getchar();

        // Если пользователь ввел символ для выхода, завершаем программу
        if (inputChar == exitChar)
        {
            break;
        }
        // Игнорируем символы новой строки (\n)
        while (getchar() != '\n');
    }
    
    return 0;
}
