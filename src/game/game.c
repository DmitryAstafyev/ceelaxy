#include "game.h"
#include "../units/player.h"
#include "../units/unit.h"
#include <SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

Game *Game_new(int height, int width) {
  Game *game = malloc(sizeof(Game));
  if (!game) {
    return NULL;
  }
  UnitList *units = UnitList_new(20, width);
  if (!units) {
    return NULL;
  }
  game->units = units;
  Player *player = Player_new(height, width);
  if (!player) {
    UnitList_destroy(game->units);
    return NULL;
  }
  game->player = player;
  return game;
}

bool Game_render(SDL_Renderer *renderer, Game *game) {
  if (!game) {
    return false;
  }
  SDL_SetRenderDrawColor(renderer, 0, 0, 32, 255);
  SDL_RenderClear(renderer);

  UnitList_render(renderer, game->units);
  Player_render(renderer, game->player);

  SDL_RenderPresent(renderer);

  return true;
}

void Game_destroy(Game *game) {
  if (!game) {
    return;
  }
  UnitList_destroy(game->units);
  Player_destroy(game->player);
  free(game);
}

void Game_run(SDL_Renderer *renderer, Game *game) {
  bool running = true;
  SDL_Event event;
  uint32_t prev = SDL_GetTicks();
  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      } else if (event.type == SDL_KEYDOWN) {
        // printf("keydown %i (%i)\n", event.type, event.key.keysym.sym);
        if (event.key.keysym.sym == SDLK_q) {
          running = false;
        } else {
          Player_events(game->player, event.key.keysym.sym);
        }
      }
    }
    if (!running) {
      break;
    }
    uint32_t tick = SDL_GetTicks();
    if (tick > prev && tick - prev < 30) {
      continue;
    }
    prev = tick;

    running = Game_render(renderer, game);

    // SDL_Delay(20);
  }
}