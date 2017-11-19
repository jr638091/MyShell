#include <fcntl.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

char *history_path;

char *itoa(int numbr) {
  int tmp = numbr;
  int count = 0;
  while (tmp != 0) {
    tmp = tmp / 10;
    ++count;
  }
  char *result = malloc(count);
  for (int i = count - 1; i >= 0; --i) {
    result[i] = numbr % 10 + 48;
    numbr = numbr / 10;
  }
  return result;
}

void show_history(int fd_destiny) {
  int history_fd = open(history_path, O_CREAT | O_RDONLY, S_IRWXU);
  char t;
  int lcount = 2;
  if (read(history_fd, &t, 1)) {
    write(fd_destiny, "1: ", 2);
    write(fd_destiny, &t, 1);
  } else
    return;
  while (read(history_fd, &t, 1)) {
    write(fd_destiny, &t, 1);
    if (t == '\n') {
      write(fd_destiny, itoa(lcount), strlen(itoa(lcount)));
      lcount++;
      write(fd_destiny, ": ", 3);
    }
  }
  write(fd_destiny, "\n", 1);
  close(history_fd);
}

void basic(int fd_destiny) {
  int history_fd = open(history_path, O_CREAT | O_RDONLY, S_IRWXU);
  char t;
  while (read(history_fd, &t, 1)) {
    write(fd_destiny, &t, 1);
  }
}

int main() {
  write(STDIN_FILENO, "hola", 4);
  char *t = malloc(4);
  read(STDIN_FILENO, t, 4);
  write(STDOUT_FILENO, t, 4);
}
