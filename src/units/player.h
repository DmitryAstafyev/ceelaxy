#ifndef PLAYER_H
#define PLAYER_H

#include "unit.h"
#include <raylib.h>
#include <stdint.h>
#include <sys/types.h>

typedef struct PlayerPosition {
  float x;
  float y;
  float z;
  float max_x;
  float max_y;
  float max_z;
  float offset_z;
  float rotate_x;
  float rotate_y;
  float rotate_z;
  float max_rotate_x;
  float max_rotate_y;
  float max_rotate_z;
  float max_angle;
  float angle;
} PlayerPosition;

typedef struct PlayerRender {
  PlayerPosition position;
  UnitSize size;
  double last_key_press;
  int direction_x_key;
  int direction_z_key;
  float acceleration;
} PlayerRender;

typedef struct Player {
  UnitType type;
  UnitState state;
  PlayerRender render;
  ShipModel *model;
} Player;

PlayerRender newPlayerRender(PlayerPosition position);

Player *newPlayer(float max_x, float max_y, float max_z, float offset_z,
                  ShipModel *model);

void drawPlayer(Player *player);

void updatePlayer(Player *player);

void destroyPlayer(Player *player);

void eventsPlayer(Player *player);

#endif