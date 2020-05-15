#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

void fprint_error_alloc() { fprintf(stderr, "trsh: allocation error\n"); }

void del_char(char *s, int i) { memmove(&s[i], &s[i + 1], strlen(s) - i); }

void free_arr(char **arr, int len) {
  int i;
  for (i = 0; i < len; i++) {
    free(arr[i]);
  }

  free(arr);
}
