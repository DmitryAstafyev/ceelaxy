#ifndef PLAYER_H
#define PLAYER_H

#include "unit.h"
#include <raylib.h>
#include <stdint.h>
#include <sys/types.h>

typedef struct {
  float x, y, z;
  float max_x, max_y, max_z;
  float offset_z;
} PlayerPosition;

typedef struct {
  float acceleration;
  int direction_x_key;
  int direction_z_key;
  double last_key_press;
} PlayerMovement;

typedef struct {
  float rotate_x, rotate_y, rotate_z;
  float rotate_step_x, rotate_step_y, rotate_step_z;
  float angle, max_angle;
  float max_rotate_x, max_rotate_y, max_rotate_z;
} PlayerVisualState;

typedef struct PlayerRender {
  PlayerPosition position;
  UnitSize size;
  PlayerMovement movement;
  PlayerVisualState state;
} PlayerRender;

typedef struct Player {
  UnitType type;
  UnitState state;
  PlayerRender render;
  ShipModel *model;
} Player;

PlayerRender newPlayerRender(float max_x, float max_y, float max_z,
                             float offset_z);

Player *newPlayer(float max_x, float max_y, float max_z, float offset_z,
                  ShipModel *model);

void drawPlayer(Player *player);

void updatePlayer(Player *player);

void destroyPlayer(Player *player);

void eventsPlayer(Player *player);

#endif