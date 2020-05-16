#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtins.h"
#include "env.h"
#include "util.h"

int builtin_exec(char **args, int argc) {
  char *cmd = args[0];
  if (strcmp(cmd, "cd") == 0) {
    return builtin_cd(args, argc);
  } else {
    return 127;
  }
}

int builtin_cd(char **args, int argc) {
  char *dir;
  int status;
  if (argc > 0) {
    const char *dest = args[1];
    char *cwd;
    cwd = getcwd(NULL, 0);
    if (cwd == NULL) {
      return 1;
    }

    dir = concat(3, cwd, "/", dest);
    free(cwd);

    status = chdir(dir);
    free(dir);
  } else {
    dir = get_home_dir();
    status = chdir(dir);
  }

  return status;
}
