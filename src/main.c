#include "./game/game.h"
#include "./utils/debug.h"
#include "raylib.h"
#include "rlgl.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
  // Check debug flag --debug
  checkDebugFlag(argc, argv);

  if (is_debug_mode)
  {
    TraceLog(LOG_INFO, "[DEBUG] Debug mode is ON");
  }

  uint32_t seed = (uint32_t) time(NULL); 
  srand(seed);
  TraceLog(LOG_INFO, "Starting");

  InitWindow(1600, 1200, "Ceelaxy");
  SetTargetFPS(60);

  Game *game = newGame(1200, 1600);

  if (!game)
  {
    return 1;
  }

  runGame(game);

  destroyGame(game);

  CloseWindow();

  return 0;
}
