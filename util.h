#ifndef UTIL_HEADER
#define UTIL_HEADER

void del_char(char *s, int i);

void fprint_error_alloc();

void arr_free(char **arr, int len);

char *concat(int count, ...);

#endif
