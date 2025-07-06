#include "player.h"
#include "../bullets/bullets.h"
#include "../utils/debug.h"
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
static const float MAX_ROTATE_X = 15.0f;
static const float MAX_ROTATE_Y = 0.0f;
static const float MAX_ROTATE_Z = 35.0f;
static const float STEP_ROTATE_X = 1.0f;
static const float STEP_ROTATE_Y = 0.0f;
static const float STEP_ROTATE_Z = 2.0f;

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
  state.rotate_step_x = STEP_ROTATE_X;
  state.rotate_step_y = STEP_ROTATE_Y;
  state.rotate_step_z = STEP_ROTATE_Z;
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
                  ShipModel *model, BulletList *bullets) {
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
  player->bullets = bullets;
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

  PlayerMovement *movement = &player->render.movement;
  PlayerVisualState *state = &player->render.state;
  PlayerPosition *position = &player->render.position;
  BulletList *bullets = player->bullets;

  double current_time = GetTime();
  double elapsed_last_bullet_spawn = current_time - bullets->last_spawn;

  if (IsKeyDown(KEY_SPACE) && elapsed_last_bullet_spawn > 0.2f) {
    Bullet bullet =
        newBullet(BULLET_MOVEMENT_DIRECTION_UP,
                  newBulletPosition(position->x, position->y,
                                    position->z + position->offset_z),
                  newBulletSize(1.0f, 1.0f, 1.0f));
    insertBulletIntoList(player->bullets, bullet);
    bullets->last_spawn = current_time;
  }
  if (!(IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_UP) ||
        IsKeyDown(KEY_DOWN))) {
    if (state->rotate_x != 0) {
      if (state->rotate_x < 0) {
        state->rotate_x += state->rotate_step_x;
      } else {
        state->rotate_x -= state->rotate_step_x;
      }
    }
    if (state->rotate_z != 0) {
      if (state->rotate_z < 0) {
        state->rotate_z += state->rotate_step_z;
      } else {
        state->rotate_z -= state->rotate_step_z;
      }
    }
    return;
  }
  double elapsed = current_time - movement->last_key_press;
  movement->last_key_press = current_time;
  if (elapsed > ACCELERATION_DEALY || directionChanged(player)) {
    movement->acceleration = ACCELERATION_INIT;
  } else {
    movement->acceleration += ACCELERATION_STEP;
  }

  if (movement->acceleration > ACCELERATION_MAX) {
    movement->acceleration = ACCELERATION_MAX;
  }

  if (IsKeyDown(KEY_LEFT)) {
    position->x -= movement->acceleration;
    movement->direction_x_key = KEY_LEFT;
    state->rotate_z -= state->rotate_step_z;
    state->rotate_z = fabsf(state->rotate_z) >= state->max_rotate_z
                          ? -state->max_rotate_z
                          : state->rotate_z;
  }
  if (IsKeyDown(KEY_RIGHT)) {
    position->x += movement->acceleration;
    movement->direction_x_key = KEY_RIGHT;
    state->rotate_z += state->rotate_step_z;
    state->rotate_z = fabsf(state->rotate_z) >= state->max_rotate_z
                          ? state->max_rotate_z
                          : state->rotate_z;
  }
  if (IsKeyDown(KEY_UP)) {
    position->z -= movement->acceleration;
    movement->direction_z_key = KEY_UP;
    state->rotate_x += state->rotate_step_x;
    state->rotate_x = fabsf(state->rotate_x) >= state->max_rotate_x
                          ? state->max_rotate_x
                          : state->rotate_x;
  }
  if (IsKeyDown(KEY_DOWN)) {
    position->z += movement->acceleration;
    movement->direction_z_key = KEY_DOWN;
    state->rotate_x -= state->rotate_step_x;
    state->rotate_x = fabsf(state->rotate_x) >= state->max_rotate_x
                          ? -state->max_rotate_x
                          : state->rotate_x;
  }
}

void drawPlayer(Player *player) {
  if (!player)
    return;
  updatePlayer(player);

  Vector3 pos = {player->render.position.x, player->render.position.y,
                 player->render.position.z + player->render.position.offset_z};

  Matrix transform = MatrixTranslate(pos.x, pos.y, pos.z);

  Matrix rotX = MatrixRotateX(DEG2RAD * player->render.state.rotate_x);

  Matrix rotZ = MatrixRotateZ(DEG2RAD * player->render.state.rotate_z);

  Matrix rotY = MatrixRotateY(DEG2RAD * 180.0f);

  Matrix result = MatrixMultiply(rotX, rotZ);
  result = MatrixMultiply(result, rotY);
  result = MatrixMultiply(result, transform);
  Model model = player->model->model;
  model.transform = result;
  DrawModel(model, (Vector3){0, 0, 0}, 1.0f, WHITE);
  if (is_debug_mode) {
    if (is_debug_mode && player->model->box_model) {
      Model box_model = *player->model->box_model;
      box_model.transform = result;
      DrawModel(box_model, (Vector3){0, 0, 0}, 1.0f, RED);
    }
  }
}

void destroyPlayer(Player *player) {
  if (!player) {
    return;
  }
  free(player);
}
