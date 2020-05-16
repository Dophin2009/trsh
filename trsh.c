#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "builtins.h"
#include "trsh.h"
#include "util.h"

int main() {
  sh_loop();
  exit(EXIT_SUCCESS);
}

void sh_loop() {
  char *line;
  char **args;
  int args_size;
  int status = 0;

  do {
    char *statusline = status_line(status);
    fprintf(stdout, "%s ", statusline);
    free(statusline);

    line = get_line();

    if (!strcmp("exit", line)) {
      free(line);
      exit(EXIT_SUCCESS);
    }

    args = parse_line(line, &args_size);
    if (!args) {
      exit(EXIT_FAILURE);
    }

    status = exec_line(args, args_size);

    free(line);
    arr_free(args, args_size);
  } while (1);
}

int exec_line(char **args, int argc) {
  if (argc <= 0) {
    return 0;
  }

  int status = builtin_exec(args, argc);
  if (status == 127) {
    pid_t pid;
    pid = fork();
    if (pid == 0) {
      if (execvp(args[0], args) == -1) {
        perror("trsh");
      }
      exit(EXIT_FAILURE);
    } else if (pid < 0) {
      perror("trsh");
    } else {
      do {
        waitpid(pid, &status, WUNTRACED);
      } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
  }

  return status;
}

char **parse_line(char *line, int *size) {
  int args_cap = 10;
  char **args = malloc(args_cap * sizeof(char *));
  int args_pos = 0;

  int current_cap = 10;
  int current_pos = 0;
  char **current_arg = &args[args_pos];
  *current_arg = malloc(current_cap * sizeof(char));
  **current_arg = '\0';

  bool escape = false;
  bool quoted = false;

  char *token;
  while ((token = strsep(&line, " ")) != NULL) {
    if (strlen(token) == 0) {
      continue;
    }

    int i;
    // Resolve escapes and check for quotes
    for (i = 0; i < strlen(token); i++) {
      char c = token[i];
      if (escape) {
        escape = false;
      } else {
        if (c == '\\') {
          escape = true;
          del_char(token, i);
          i--;
        } else if (c == '"') {
          quoted = !quoted;
          del_char(token, i);
          i--;
        }
      }
    }

    // Concatenate token to current arg
    int token_len = strlen(token);
    int current_len = strlen(*current_arg);
    if (current_len + token_len > current_cap) {
      current_cap = current_len + token_len + 1;
      *current_arg = realloc(*current_arg, current_cap * sizeof(char));
      if (!*current_arg) {
        fprint_error_alloc();
        exit(EXIT_FAILURE);
      }
    };
    strncat(*current_arg, token, token_len);
    current_pos += token_len;

    if (!quoted && !escape) {
      (*current_arg)[current_pos] = '\0';

      ++args_pos;

      // If exceeded arg array size, increase size
      if (args_pos >= args_cap) {
        args_cap = 10;
        args = realloc(args, args_cap * sizeof(char *));
        if (!args) {
          fprint_error_alloc();
          exit(EXIT_FAILURE);
        }
      }

      // Allocate space for new arg
      current_cap = 10;
      current_pos = 0;
      current_arg = &args[args_pos];
      *current_arg = malloc(current_cap * sizeof(char));
      if (!args[args_pos]) {
        fprint_error_alloc();
        exit(EXIT_FAILURE);
      }

      **current_arg = '\0';
    } else {
      if (current_pos >= current_cap) {
        current_cap += 1;
        *current_arg = realloc(*current_arg, current_cap * sizeof(char));
        if (!*current_arg) {
          fprint_error_alloc();
          exit(EXIT_FAILURE);
        }
      }
      (*current_arg)[current_pos] = ' ';
      (*current_arg)[++current_pos] = '\0';
    }
  }

  free(token);

  if (strlen(args[args_pos]) != 0) {
    ++args_pos;
    if (args_pos >= args_cap) {
      ++args_cap;
      args = realloc(args, args_cap * sizeof(char *));
      if (!args) {
        fprint_error_alloc();
        exit(EXIT_FAILURE);
      }
    }
  } else {
    free(args[args_pos]);
  }
  args[args_pos] = NULL;

  *size = args_pos + 1;
  return args;
}

#define BUFSIZE 1024
char *get_line() {
  unsigned int buf_size = BUFSIZE;
  char *buffer = malloc(buf_size * sizeof(char));
  if (!buffer) {
    fprint_error_alloc();
    exit(EXIT_FAILURE);
  }

  unsigned int pos = 0;
  while (1) {
    char c = getchar();
    if (c == '\n' || c == EOF) {
      buffer[pos] = '\0';
      return buffer;
    }
    buffer[pos] = c;
    pos++;

    if (pos >= buf_size) {
      buf_size += BUFSIZE;
      buffer = realloc(buffer, buf_size * sizeof(char));
      if (!buffer) {
        fprint_error_alloc();
        exit(EXIT_FAILURE);
      }
    }
  }

  return buffer;
}

char *status_line(int status) {
  char *cwd = getcwd(NULL, 0);
  char *line = concat(2, cwd, " >");
  free(cwd);

  if (line == NULL) {
    fprint_error_alloc();
    exit(EXIT_FAILURE);
  }
  return line;
}
