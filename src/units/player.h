#ifndef PLAYER_H
#define PLAYER_H

#include "unit.h"
#include <raylib.h>
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

PlayerRender newPlayerRender(UnitPosition position, int height, int width);

Player *newPlayer(int height, int width);

void destroyPlayer(Player *player);

Matrix getPlayerMatrix(Player *player);

void eventsPlayer(Player *player);

#endif