#include "player.h"
#include "unit.h"
#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

static const float ACCELERATION_DEALY = 0.2f;
static const float ACCELERATION_INIT = 0.1f;
static const float ACCELERATION_STEP = 0.05f;
static const float ACCELERATION_MAX = 1.0f;
static const float MAX_ROTATE_X = 10.0f;
static const float MAX_ROTATE_Y = 0.0f;
static const float MAX_ROTATE_Z = 15.0f;

PlayerPosition newPlayerPosition(float max_x, float max_y, float max_z,
                                 float offset_z) {
  PlayerPosition position;
  position.x = 0.0f;
  position.y = 0.0f;
  position.z = 0.0f;
  position.max_x = max_x;
  position.max_y = max_y;
  position.max_z = max_z;
  position.offset_z = offset_z;
  return position;
};

PlayerVisualState newPlayerVisualState(float offset_z) {
  PlayerVisualState state;
  state.max_rotate_x = MAX_ROTATE_X;
  state.max_rotate_y = MAX_ROTATE_Y;
  state.max_rotate_z = MAX_ROTATE_Z;
  state.rotate_x = 0.0f;
  state.rotate_y = 0.0f;
  state.rotate_z = 0.0f;
  state.max_angle = 15.0f;
  state.angle = 0.0f;
  return state;
};

PlayerMovement newPlayerMovement() {
  PlayerMovement movement;
  movement.last_key_press = GetTime();
  movement.acceleration = 0;
  movement.direction_x_key = 0;
  movement.direction_z_key = 0;
  return movement;
}

PlayerRender newPlayerRender(float max_x, float max_y, float max_z,
                             float offset_z) {
  PlayerRender render;
  render.position = newPlayerPosition(max_x, max_y, max_z, offset_z);
  render.size = newUnitSize();
  render.state = newPlayerVisualState(offset_z);
  render.movement = newPlayerMovement();
  return render;
}

Player *newPlayer(float max_x, float max_y, float max_z, float offset_z,
                  ShipModel *model) {
  if (!model) {
    return NULL;
  }
  Player *player = malloc(sizeof(Player));
  if (!player) {
    return NULL;
  }
  player->type = UNIT_TYPE_SOLDER;
  player->state = newUnitState();
  player->render = newPlayerRender(max_x, max_y, max_z, offset_z);
  player->model = model;
  return player;
}

bool directionChanged(Player *player) {
  return (IsKeyDown(KEY_LEFT) &&
          player->render.movement.direction_x_key != KEY_LEFT) ||
         (IsKeyDown(KEY_RIGHT) &&
          player->render.movement.direction_x_key != KEY_RIGHT) ||
         (IsKeyDown(KEY_UP) &&
          player->render.movement.direction_z_key != KEY_UP) ||
         (IsKeyDown(KEY_DOWN) &&
          player->render.movement.direction_z_key != KEY_DOWN);
}

void updatePlayer(Player *player) {
  if (!player) {
    return;
  }
  if (!(IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_UP) ||
        IsKeyDown(KEY_DOWN))) {
    return;
  }
  double current_time = GetTime();
  double elapsed = current_time - player->render.movement.last_key_press;
  player->render.movement.last_key_press = current_time;
  if (elapsed > ACCELERATION_DEALY || directionChanged(player)) {
    player->render.movement.acceleration = ACCELERATION_INIT;
  } else {
    player->render.movement.acceleration += ACCELERATION_STEP;
  }

  if (player->render.movement.acceleration > ACCELERATION_MAX) {
    player->render.movement.acceleration = ACCELERATION_MAX;
  }

  if (IsKeyDown(KEY_LEFT)) {
    player->render.position.x -= player->render.movement.acceleration;
    player->render.movement.direction_x_key = KEY_LEFT;
  }
  if (IsKeyDown(KEY_RIGHT)) {
    player->render.position.x += player->render.movement.acceleration;
    player->render.movement.direction_x_key = KEY_RIGHT;
  }
  if (IsKeyDown(KEY_UP)) {
    player->render.position.z -= player->render.movement.acceleration;
    player->render.movement.direction_z_key = KEY_UP;
  }
  if (IsKeyDown(KEY_DOWN)) {
    player->render.position.z += player->render.movement.acceleration;
    player->render.movement.direction_z_key = KEY_DOWN;
  }
}

void drawPlayer(Player *player) {
  if (!player) {
    return;
  }

  DrawModelEx(
      player->model->model,
      (Vector3){player->render.position.x, player->render.position.y,
                player->render.position.z + player->render.position.offset_z},
      (Vector3){0, 1, 0}, 180.0f, (Vector3){1, 1, 1}, WHITE);
  // DrawModelEx(
  //     player->model->model,
  //     (Vector3){player->render.position.x, player->render.position.y,
  //               player->render.position.z +
  //               player->render.position.offset_z},
  //     (Vector3){player->render.position.rotate_x,
  //               player->render.position.rotate_y,
  //               player->render.position.rotate_z},
  //     player->render.position.angle, (Vector3){1, 1, 1}, WHITE);
}

void destroyPlayer(Player *player) {
  if (!player) {
    return;
  }
  free(player);
}
