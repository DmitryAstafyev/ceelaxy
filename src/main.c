#include "./game/game.h"
#include "./utils/debug.h"
#include "raylib.h"
#include "rlgl.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
  // Check debug flag --debug
  checkDebugFlag(argc, argv);

  if (is_debug_mode) {
    printf("[DEBUG] Debug mode is ON\n");
  }

  srand(time(NULL));
  printf("Starting\n");

  InitWindow(1600, 1200, "Ceelaxy");
  SetTargetFPS(60);

  Game *game = newGame(1200, 1600);

  if (!game) {
    return 1;
  }

  runGame(game);

  destroyGame(game);

  CloseWindow();

  return 0;
}
