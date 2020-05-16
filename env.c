#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *get_home_dir() {
  char *dir;
#ifdef _WIN32
  homedrive = getenv("HOMEDRIVE");
  homepath = getenv("HOMEPATH");
  dir = malloc((strlen(homedrive) + strlen(homepath)) * sizeof(char));
  dir = strncpy(dir, homedrive, strlen(homedrive));
  dir = strncat(dir, homepath, strlen(homepath));
#else
  dir = getenv("HOME");
#endif
  return dir;
}
