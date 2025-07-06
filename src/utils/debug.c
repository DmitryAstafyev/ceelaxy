#include "debug.h"
#include <string.h>

bool is_debug_mode = false;

void checkDebugFlag(int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--debug") == 0) {
      is_debug_mode = true;
      break;
    }
  }
}