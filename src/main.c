#include "./game/game.h"
#include "./utils/debug.h"
#include "./utils/resolution.h"
#include "raylib.h"
#include "rlgl.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
  // Check debug flag --debug
  checkDebugFlag(argc, argv);

  // Check resolution flag --resolution or -r
  checkResolution(argc, argv);

  if (is_debug_mode)
  {
    TraceLog(LOG_INFO, "[DEBUG] Debug mode is ON");
  }

  uint32_t seed = (uint32_t)time(NULL);
  srand(seed);
  TraceLog(LOG_INFO, "Starting");

  InitWindow(resolution_width, resolution_height, "Ceelaxy");
  SetTargetFPS(60);

  Game *game = newGame(resolution_height, resolution_width);

  if (!game)
  {
    return 1;
  }

  runGame(game);

  destroyGame(game);

  CloseWindow();

  return 0;
}
