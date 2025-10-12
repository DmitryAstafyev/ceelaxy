#include <string.h>
#include "raylib.h"
#include "resolution.h"
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

int resolution_width = 1600;
int resolution_height = 1200;

bool parse_int(const char *s, int *out)
{
  if (!s || !out)
    return false;

  errno = 0;
  char *end = NULL;
  long v = strtol(s, &end, 10);
  if (end == s)
    return false;

  while (*end != '\0' && isspace((unsigned char)*end))
    end++;

  if (*end != '\0')
    return false;
  if (errno == ERANGE || v < INT_MIN || v > INT_MAX)
    return false;

  *out = (int)v;
  return true;
}

/**
 * @brief Parses command-line arguments to set the resolution.
 *
 * Looks for "--resolution" or "-r" followed by a width value.
 * Sets resolution_width and resolution_height if valid.
 * Ensures width is within MIN_WIDTH and MAX_WIDTH bounds.
 *
 * @param argc Argument count from main.
 * @param argv Argument vector from main.
 */
void checkResolution(int argc, char *argv[])
{
  for (int i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "--resolution") == 0 || strcmp(argv[i], "-r") == 0)
    {
      if (i + 1 < argc)
      {
        int width;

        if (!parse_int(argv[i + 1], &width))
        {
          TraceLog(LOG_WARNING, "Invalid resolution width '%s'. Using default %u.",
                   argv[i + 1], resolution_width);
          return;
        }
        if (width >= MIN_WIDTH && width <= MAX_WIDTH)
        {
          resolution_width = width;
          resolution_height = (int)((float)width * WIDTH_HEIGHT_RATIO);
          TraceLog(LOG_INFO, "Setting resolution to %ux%u", resolution_width, resolution_height);
        }
        else
        {
          TraceLog(LOG_WARNING, "Resolution width %u is out of bounds (%u - %u). Using default %u.",
                   width, MIN_WIDTH, MAX_WIDTH, resolution_width);
        }
      }
      break;
    }
  }
}