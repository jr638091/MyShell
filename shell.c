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
char *user_name;
struct proccess *first;
int c;
char *code;
char *history_path;
int pid;
char *user_dir;

// Methods declarations
void begin();
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
int parser(char *cmd);
int ampersan(char *cmd);
int special_character(char c);
int next_token(char *token, int *index, char *str, int ichange);
int close_input(int fd);
int close_output(int fd);
int againer(char *str);
int find_line_history(char *result, int line);
int isnumber(int *number, char *str, int index);
int expander(char *str, char *paster, int i, int j);
int parser(char *str);
void change_directory(char *dir);

// My handlers
void handlerctrC(int a) {
  if (pid > 0) {
    kill(pid, 9);
    rm_proccess(pid);
    return;
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
  signal(SIGINT, handlerctrC);
  signal(SIGCHLD, handler_end_proccess);

  if (cargs == 1)
    user_name = "snow\0";

  else
    user_name = vargs[1];

  begin();
  return 0;
}

void limpiar(char *cmd, int count) {
  for (int i = 0; i < count; i++) {
    cmd[i] = '\0';
  }
}

void make_user_dir(char *user_dir, char *user_name) {
  for (int i = 0; i < strlen(user_name); ++i) {
    user_dir[6 + i] = user_name[i];
  }
}

void begin() {
  while (1) {
    char *cwd = malloc(500);
    getcwd(cwd, 500);
    pid = 0;
    user_dir = malloc(100);
    limpiar(user_dir, 100);
    strcpy(user_dir, "/home/");
    // make_user_dir(user_dir, user_name);
    strcpy(user_dir + 6, user_name);
    int len = strlen(user_dir);
    if (strncmp(cwd, user_dir, len) == 0) {
      user_dir[0] = '~';

      strcpy(user_dir + 1, cwd + len);
      fprintf(stderr, "\e[1;31m%s@%s\e[0;0m:\e[2;36m%s\e[0;0m$", user_name,
              user_name, user_dir);
    } else {
      fprintf(stderr, "\e[1;31m%s@%s\e[0;0m:\e[2;36m%s\e[0;0m$", user_name,
              user_name, cwd);
    }

    limpiar(code, 500);
    leer();
    if (strlen(code) != 0 && againer(code) == 0) {
      write_history(code);
      if (!ampersan(code))
        parser(code);
    }
    free(cwd);
    free(user_dir);
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
      change_directory(args[1]);
      if (chdir(args[1]) == -1)
        write(STDOUT_FILENO, "file not found!\n", strlen("file not found!\n"));
      return;
    }

    if (strcmp(args[0], "exit") == 0) {
      exit(0);
    }

    pid = fork();
    if (pid > 0) {
      int status = 0;
      int f = waitpid(pid, &status, 0);
    } else if (!pid) {

      if (execute_cmd(args, fd_source, fd_destiny) == -1) {
        write(STDOUT_FILENO, "command not found\n",
              strlen("command not found\n"));
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
    new_proccess->next = NULL;
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
    new_proccess->next->next = NULL;
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
  if (to_remove != NULL && to_remove->pid == pid && to_remove->next == NULL) {
    first = NULL;
    return;
  }
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
  char *result = malloc(count + 1);
  limpiar(result, count);
  for (int i = count - 1; i >= 0; --i) {
    result[i] = numbr % 10 + 48;
    numbr = numbr / 10;
  }
  result[count] = '\0';
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
  if (cmd[0] == ' ' && strlen(code) != 0)
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

int ampersan(char *cmd) {
  int i = strlen(cmd) - 1;
  while (cmd[i] == ' ') {
    cmd[i] = '\0';
    --i;
  }
  if (cmd[i] == '&') {
    cmd[i] = '\0';
    pid = fork();
    if (pid > 0) {
      add_proccess(pid, cmd);
      return 1;
    } else {
      parser(cmd);
      exit(0);
    }
  }
  return 0;
}

int special_character(char c) {

  return (c == '|' || c == ';' || c == '>' || c == '<');
}

int next_token(char *token, int *index, char *str, int ichange) {

  static int i = 0;
  if (ichange == 1) {
    i = 0;
    return 0;
  } else if (ichange < 0) {
    i += ichange;
    return 0;
  }

  bzero(token, 100);

  int comillas = 0;
  char c;
  while (str[i] == ' ')
    i++;

  if (str[i] == 0) {
    *token = 0;
    return 0;
  }

  if (special_character(str[i]))
    if (str[i] == '>' && str[i + 1] == '>') {
      token = ">>";
      i += 2;
      return 2;
    } else {
      token[0] = str[i++];
      return 2;
    }

  int j = 0;
  while (comillas ||
         (str[i] != ' ' && str[i] != 0 && !special_character(str[i]))) {

    if (str[i] == '"' || str[i] == 39) // 39 = '
      comillas = !comillas;
    else
      token[j++] = str[i++];
  }

  if (comillas)
    return -1;

  *index = i;
  return 1;
}

int close_input(int fd) {
  if (fd != 0)
    close(fd);
}

int close_output(int fd) {
  if (fd != 1)
    close(fd);
}

int againer(char *str) {

  int index;
  char token[100];
  int n = 0;
  next_token(NULL, NULL, 0, 1); // inializa el i del next_token

  int iscommand = 1;

  while (1) {
    n = next_token(token, &index, str, 0);

    if (n == 0)
      return 0;

    if (iscommand && n != 1) {
      write(1, "Orden no encontrada\n", strlen("Orden no encontrada\n"));
      return -1;
    }

    if (n == 1 && iscommand && !strncmp(token, "again", 5)) {

      int number;
      int j;
      if ((j = isnumber(&number, str, index)) == -1) {
        write(1, "Error en el comando again\n",
              strlen("Error en el comando again\n"));
        return -1;
      }
      char line[500];
      if (find_line_history(line, number) == -1) {
        write(1, "La linea no existe\n", strlen("La linea no existe\n"));
        return -1;
      }
      expander(str, line, index - 5, j);
      next_token(NULL, NULL, 0, -5);
    }

    if (n == 1)
      iscommand = 0;
    if (n == 2 && (!strncmp(token, "|", 1) || !strncmp(token, ";", 1)))
      iscommand = 1;
  }
}

int find_line_history(char *result, int line) {

  int fd = open(history_path, O_RDONLY, S_IRWXU);
  int i = 1;
  char c;
  while (i < line) {
    while (read(fd, &c, 1) && c != '\n')
      ;
    i++;
  }
  if (i < line) {
    close(fd);
    return -1;
  }
  i = 0;
  while (read(fd, &c, 1) && c != '\n')
    result[i++] = c;
  result[i] = 0;
  close(fd);
  return 0;
}

int isnumber(int *number, char *str, int index) {

  int i = index;
  int s = 0;
  while (str[i] == ' ') {
    i++;
    s = 1;
  }
  if (!s)
    return -1;
  char n[100];
  limpiar(n, 100);
  int j = 0;
  s = 0;
  while (str[j + i] >= 48 && str[j + i] <= 57) {
    s = 1;
    n[j] = str[i + j];
    j++;
  }
  if (!s)
    return -1;
  if (str[j + i] != 0 && str[j + i] != ' ' && !special_character(str[j]))
    return -1;
  *number = atoi(n);

  return i + j;
}

int expander(char *str, char *paster, int i, int j) {

  char rest[100];
  strcpy(rest, str + j);
  strcpy(str + i, paster);
  strcpy(str + i + strlen((paster)), rest);
  return 0;
}

int parser(char *str) {

  char c;
  int i = 0;
  int index;
  char *token = malloc(100);
  int ichange = 0;
  int n = 0;
  next_token(NULL, NULL, 0, 1); // inializa el i del next_token

  int iscommand = 1;
  char command[100];
  char *argv[10];
  int argc = 0;
  int input = 0;
  int output = 1;

  while (1) {
    n = next_token(token, &index, str, ichange);
    if (iscommand && n != 1) {
      write(1, "Orden no encontrada\n", strlen("Orden no encontrada\n"));
      return -1;
    }

    switch (n) {
    case 1: // caso en que sea un comando o un parametro
      if (iscommand) {
        strcpy(command, token);
        argv[argc] = malloc(100);
        strcpy(argv[argc++], token);
        iscommand = 0;
      } else {
        argv[argc] = malloc(100);
        strcpy(argv[argc++], token);
      }
      break;

    case 2:                          // caso en que sea un caracter especial
      if (!strncmp(token, "|", 1)) { // si es un pipe

        int p[2];
        pipe(p);
        argv[argc] = NULL;
        execute(argv, input, p[1]);
        close(p[1]);
        close_input(input);

        output = 1;
        input = p[0];
        iscommand = 1;
        argc = 0;
      }

      else if (!strncmp(token, "<", 1)) {

        if (next_token(token, &index, str, ichange) != 1) {
          write(1, "Error en el archivo esperado\n",
                strlen("Error en el archivo esperado\n"));
          return -1;
        }
        close_input(input);
        input = open(token, O_RDONLY, 0);

      }

      else if (!strncmp(token, ">>", 2)) {

        if (next_token(token, &index, str, ichange) != 1) {
          write(1, "Error en el archivo esperado\n",
                strlen("Error en el archivo esperado\n"));
          return -1;
        }
        close_output(output);
        output = open(token, O_WRONLY | O_CREAT | O_APPEND, 0666);
      }

      else if (!strncmp(token, ">", 1)) {

        if (next_token(token, &index, str, ichange) != 1) {
          write(1, "Error en el archivo esperado\n",
                strlen("Error en el archivo esperado\n"));
          return -1;
        }
        close_output(output);
        output = open(token, O_WRONLY | O_CREAT | O_TRUNC, 0666);
      } else if (!strncmp(token, ";", 1)) {
        argv[argc] = NULL;
        execute(argv, input, output);
        close_input(input);
        close_output(output);

        output = 1;
        input = 0;
        iscommand = 1;
        argc = 0;
      }
      break;

    case 0:
      argv[argc] = NULL;
      execute(argv, input, output);
      close_input(input);
      close_output(output);

      output = 1;
      input = 0;
      iscommand = 1;
      argc = 0;
      return 0;
      break;
    case -1:
      return -1;
      break;
    }
  }
}

void change_directory(char *dir) {
  char *temp = malloc(500);
    limpiar(user_dir,100);
    strcpy(user_dir,"/home/");
    make_user_dir(user_dir,user_name);
  for (int i = 0; i < strlen(dir); ++i) {
    if (dir[i] == '~') {
      strcpy(temp, dir + i + 1);
      strcpy(dir + i, user_dir);
      strcpy(dir + i + strlen(user_dir), temp);
      limpiar(temp, 500);
    }
  }
  free(temp);
}