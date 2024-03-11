#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <windows.h>

int main(int argc, char *argv[]) {
  if (argc != 3) {
    char error_msg[] = "Usage: <input_file> <output_file>\n";
    WriteConsoleA(GetStdHandle(STD_ERROR_HANDLE), error_msg, strlen(error_msg), NULL, NULL);
    return 1;
  }

  int input_fd = _open(argv[1], _O_RDONLY | _O_BINARY, _S_IREAD);
  if (input_fd == -1) {
    char error_msg[100];
    DWORD errorMessageID = GetLastError();
    LPSTR messageBuffer = NULL;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
    snprintf(error_msg, 100, "Error opening input file: %s", messageBuffer);
    LocalFree(messageBuffer);
    WriteConsoleA(GetStdHandle(STD_ERROR_HANDLE), error_msg, strlen(error_msg), NULL, NULL);
    return 1;
  }

  int output_fd = _open(argv[2], _O_WRONLY | _O_BINARY | _O_CREAT | _O_APPEND, _S_IWRITE);
  if (output_fd == -1) {
    char error_msg[100];
    DWORD errorMessageID = GetLastError();
    LPSTR messageBuffer = NULL;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
    snprintf(error_msg, 100, "Error opening or creating output file: %s", messageBuffer);
    LocalFree(messageBuffer);
    WriteConsoleA(GetStdHandle(STD_ERROR_HANDLE), error_msg, strlen(error_msg), NULL, NULL);
    _close(input_fd);
    return 1;
  }

  char buf[BUFSIZ];
  int bytesRead;
  while ((bytesRead = _read(input_fd, buf, BUFSIZ)) > 0) {
    if (_write(output_fd, buf, bytesRead) != bytesRead) {
      char error_msg[100];
      DWORD errorMessageID = GetLastError();
      LPSTR messageBuffer = NULL;
      FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                     NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
      snprintf(error_msg, 100, "Error writing to output file: %s", messageBuffer);
      LocalFree(messageBuffer);
      WriteConsoleA(GetStdHandle(STD_ERROR_HANDLE), error_msg, strlen(error_msg), NULL, NULL);
      _close(input_fd);
      _close(output_fd);
      return 1;
    }
  }

  if (bytesRead == -1) {
    char error_msg[100];
    DWORD errorMessageID = GetLastError();
    LPSTR messageBuffer = NULL;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
    snprintf(error_msg, 100, "Error reading input file: %s", messageBuffer);
    LocalFree(messageBuffer);
    WriteConsoleA(GetStdHandle(STD_ERROR_HANDLE), error_msg, strlen(error_msg), NULL, NULL);
    _close(input_fd);
    _close(output_fd);
    return 1;
  }

  _close(input_fd);
  _close(output_fd);
  return 0;
}
