#include "path.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define the path separator based on the operating system
char *path_join(const char *base, const char *joined)
{
  size_t len = strlen(base) + strlen(joined) + 2;
  char *result = malloc(len);
  if (!result)
  {
    return NULL;
  }
  snprintf(result, len, "%s%c%s", base, PATH_SEP, joined);
  return result;
}