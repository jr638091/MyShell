#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// My structs
struct proccess {
  int pid;
  char *cmd;
  struct proccess *next;
};

// varibles declaretions
struct proccess *first;
int c;
char *code;
char *history_path;
int pid;

// Methods declarations
void non_specific_user();
void leer();
void execute(char *args[], int fd_source, int fd_destiny);
int execute_cmd(char *args[], int fd_source, int fd_destiny);
void add_proccess(int pid, char *cmd);
struct proccess *find_proccess(int pid);
void rm_proccess(int pid);
void show_proccess(int fd_destiny);
void show_history(int fd_destiny);
void write_history(char *cmd);
char *itoa(int numbr);

// My handlers
void handlerctrC(int a) {
  if (pid > 0) {
    kill(pid, 9);
    rm_proccess(pid);
  } else {
    write(STDOUT_FILENO, "\n", 1);
    exit(0);
  }
}

void handler_end_proccess(int a) {
  int pid;
  int status;
  while ((pid = waitpid(-1, &status, WNOHANG)) != -1) {
    rm_proccess(pid);
  }
}

char *get_history_path() {
  char *t = malloc(500);
  getcwd(t, 500);
  strcpy(t + strlen(t), "/.history.txt");
  return t;
}

int main(int cargs, char *vargs[]) {
  history_path = get_history_path();
  code = malloc(500);
  // leer();
  // write(STDOUT_FILENO,code,c);

  signal(SIGINT, handlerctrC);
  signal(SIGCHLD, handler_end_proccess);

  if (cargs == 1) {
    non_specific_user();
  }

  return 0;
}

void non_specific_user() {
  while (1) {
    char *cwd = malloc(500);
    getcwd(cwd, 500);
    pid = 0;
    write(STDOUT_FILENO, cwd, strlen(cwd));
    write(STDOUT_FILENO, "~:", 2);
    leer();
    write_history(code);
    int count = 1;
    char *token = strtok(code, " ");
    char *b[500];
    b[0] = token;
    while ((token = strtok(NULL, " "))) {
      b[count] = token;
      ++count;
    }
    char *args[count + 1];
    for (int i = 0; i < count; i++) {
      args[i] = b[i];
    }
    args[count] = NULL;
    execute(args, 0, 1);
  }
}

void leer() {
  char t;
  c = 0;
  while (read(STDIN_FILENO, &t, 1)) {
    if (t != '\n') {
      code[c++] = t;
    } else {
      code[c++] = '\0';
      break;
    }
  }
}

void execute(char *args[], int fd_source, int fd_destiny) {
  if (strcmp(code, "\0") != 0) {

    if (strcmp(args[0], "jobs") == 0) {
      show_proccess(fd_destiny);
      return;
    }

    if (strcmp(args[0], "history") == 0) {
      show_history(fd_destiny);
      return;
    }
    if (strcmp(args[0], "cd") == 0) {
      if (chdir(args[1]) == -1)
        write(STDOUT_FILENO, "file not found!", strlen("file not found!"));
      return;
    }

    pid = fork();
    if (pid > 0) {
      // add_proccess(pid, args[0]);
      int status = 0;
      int f = waitpid(pid, &status, 0);
    } else if (!pid) {

      if (execute_cmd(args, fd_source, fd_destiny) == -1) {
        write(STDOUT_FILENO, "command not found\n",
              sizeof("command not found\n"));
      }

      exit(0);
    }
  }
}

int execute_cmd(char *args[], int fd_source, int fd_destiny) {
  dup2(fd_source, 0);
  dup2(fd_destiny, 1);
  int ret = execvp(args[0], args);
  return ret;
}

void add_proccess(int pid, char *cmd) {
  struct proccess *new_proccess = first;
  if (new_proccess == NULL) {
    new_proccess = malloc(sizeof(struct proccess));
    new_proccess->cmd = malloc(strlen((cmd)));
    strcpy(new_proccess->cmd, cmd);
    new_proccess->pid = pid;
    first = new_proccess;
  } else {
    while (new_proccess->next != NULL) {
      new_proccess = new_proccess->next;
    }
    new_proccess->next = malloc(sizeof(struct proccess));
    new_proccess->cmd = malloc(strlen((cmd)));
    strcpy(new_proccess->cmd, cmd);
    new_proccess->next->cmd = cmd;
    new_proccess->next->pid = pid;
  }
}

struct proccess *find_proccess(int pid) {
  struct proccess *temporal_proccess = first;
  if (temporal_proccess == NULL)
    return NULL;
  if (temporal_proccess->pid == pid)
    return temporal_proccess;
  while (temporal_proccess->next != NULL) {
    if (temporal_proccess->next->pid == pid)
      return temporal_proccess;
    temporal_proccess = temporal_proccess->next;
  }
  return NULL;
}

void rm_proccess(int pid) {
  struct proccess *to_remove = find_proccess(pid);
  if (to_remove != NULL) {
    if (to_remove->pid == pid)
      first = first->next;
    else
      to_remove->next = to_remove->next->next;
  }
}

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

void show_proccess(int fd_destiny) {
  struct proccess *to_print = first;
  while (to_print != NULL) {
    write(fd_destiny, itoa(to_print->pid), strlen(itoa(to_print->pid)));
    write(fd_destiny, "      ", 1);
    write(fd_destiny, to_print->cmd, strlen(to_print->cmd));
    write(fd_destiny, "\n", 1);
    to_print = to_print->next;
  }
}

void show_history(int fd_destiny) {
  int history_fd = open(history_path, O_CREAT | O_RDONLY, S_IRWXU);
  char t;
  int lcount = 2;
  if (read(history_fd, &t, 1)) {
    write(fd_destiny, "1: ", 3);
    write(fd_destiny, &t, 1);
  } else
    return;
  while (read(history_fd, &t, 1)) {
    write(fd_destiny, &t, 1);
    if (t == '\n') {
      write(fd_destiny, itoa(lcount), strlen(itoa(lcount)));
      lcount++;
      write(fd_destiny, ": ", 2);
    }
  }
  write(fd_destiny, "\n", 1);
  close(history_fd);
}

void write_history(char *cmd) {
  if (cmd[0] == ' ')
    return;
  char t;
  int history_fd = open(history_path, O_CREAT | O_RDWR, S_IRWXU);
  if (read(history_fd, &t, 1) == 0) {
    write(history_fd, cmd, strlen(cmd));
    return;
  }
  lseek(history_fd, 0, SEEK_END);
  write(history_fd, "\n", 1);
  write(history_fd, cmd, strlen(cmd));
  close(history_fd);
}