#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[]) {
  if (argc !=3) {
    write(STDERR_FILENO, "Usage: <input_file> <output_file>\n", 33);
    return 1;
  }
  if (argc < 3) {
    write(STDERR_FILENO, "Usage: <input_file> <output_file>\n", 33);
    return 1;
  }

  int input_fd = open(argv[1], O_RDONLY);
  if (input_fd == -1) {
    char error_msg[50];
    sprintf(error_msg, "Error opening input file: %s\n", strerror(errno));
    write(STDERR_FILENO, error_msg, strlen(error_msg));
    return 1;
  }

  int output_fd = open(argv[2], O_WRONLY | O_CREAT | O_APPEND, 0644);
  if (output_fd == -1) {
    char error_msg[50];
    sprintf(error_msg, "Error opening or creating output file: %s\n", strerror(errno));
    write(STDERR_FILENO, error_msg, strlen(error_msg));
    close(input_fd);
    return 1;
  }

  char buf[BUFSIZ];
  ssize_t bytesRead;
  while ((bytesRead = read(input_fd, buf, BUFSIZ)) > 0) {
    if (write(output_fd, buf, bytesRead) != bytesRead) {
      char error_msg[50];
      sprintf(error_msg, "Error writing to output file: %s\n", strerror(errno));
      write(STDERR_FILENO, error_msg, strlen(error_msg));
      close(input_fd);
      close(output_fd);
      return 1;
    }
  }

  if (bytesRead == -1) {
    char error_msg[50];
    sprintf(error_msg, "Error reading input file: %s\n", strerror(errno));
    write(STDERR_FILENO, error_msg, strlen(error_msg));
    close(input_fd);
    close(output_fd);
    return 1;
  }

  close(input_fd);
  close(output_fd);
  return 0;
}
