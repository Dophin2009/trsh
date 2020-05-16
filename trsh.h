#ifndef TRSH_HEADER
#define TRSH_HEADER

void sh_loop();

char *get_line();
char **parse_line(char *line, int *size);
int exec_line(char **args, int argc);

char *status_line(int status);

#endif
