#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

void fprint_error_alloc() { fprintf(stderr, "trsh: allocation error\n"); }

void del_char(char *s, int i) { memmove(&s[i], &s[i + 1], strlen(s) - i); }

void arr_free(char **arr, int len) {
  int i;
  for (i = 0; i < len; i++) {
    free(arr[i]);
  }

  free(arr);
}

char *concat(int count, ...) {
  va_list ap;
  int i;

  int len = 1;
  va_start(ap, count);
  for (i = 0; i < count; i++)
    len += strlen(va_arg(ap, char *));
  va_end(ap);

  char *merged = calloc(sizeof(char), len);
  int null_pos = 0;

  va_start(ap, count);
  for (i = 0; i < count; i++) {
    char *s = va_arg(ap, char *);
    strcpy(merged + null_pos, s);
    null_pos += strlen(s);
  }
  va_end(ap);

  return merged;
}
