#ifndef GAME_H
#define GAME_H

#include "../units/player.h"
#include "../units/unit.h"
#include <SDL.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct Game {
  UnitList *units;
  Player *player;
} Game;

Game *Game_new(int height, int width);

bool Game_render(SDL_Renderer *renderer, Game *game);

void Game_destroy(Game *game);

void Game_run(SDL_Renderer *renderer, Game *game);

#endif