#include "./game/game.h"
#include "./raylib/rlights.h"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
  srand(time(NULL));
  printf("Starting\n");
  InitWindow(800, 600, "Ceelaxy");
  SetTargetFPS(60);

  Game *game = newGame(600, 800);

  if (!game) {
    return 1;
  }

  runGame(game);

  destroyGame(game);
  CloseWindow();

  return 0;
}
