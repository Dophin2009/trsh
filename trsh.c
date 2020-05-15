#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

void sh_loop();
char *get_line();
char **parse_line(char *line, int *size);
/* int exec_line(char **args); */

int main() {
  sh_loop();
  exit(EXIT_SUCCESS);
}

void sh_loop() {
  while (1) {
    fprintf(stdout, "> ");
    char *line = get_line();

    if (!strcmp("exit", line)) {
      free(line);
      exit(EXIT_SUCCESS);
    }

    int args_size;
    char **args = parse_line(line, &args_size);
    if (!args) {
      exit(EXIT_FAILURE);
    }

    for (int i = 0; i < args_size; i++) {
      printf("%d: %s\n", i, args[i]);
    }

    /* int status = exec_line(args); */
    /* if (!status) { */
    /* break; */
    /* } */

    free(line);

    free_arr(args, args_size);
  }
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

  if (strlen(args[args_pos]) == 0) {
    free(args[args_pos]);
    --args_pos;
  }

  free(token);

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
