#ifndef PLAYER_H
#define PLAYER_H

#include "SDL_stdinc.h"
#include "unit.h"
#include <SDL.h>
#include <stdint.h>
#include <sys/types.h>

typedef struct PlayerRender {
  UnitPosition position;
  UnitSize size;
  UnitSize window;
  uint32_t last_key_press;
  uint32_t acceleration;
} PlayerRender;

typedef struct Player {
  UnitType type;
  UnitState state;
  PlayerRender render;
} Player;

PlayerRender PlayerRender_new(UnitPosition position, int height, int width);

Player *Player_new(int height, int width);

void Player_destroy(Player *player);

void Player_render(SDL_Renderer *renderer, Player *player);

void Player_events(Player *player, SDL_Keycode sym);

#endif