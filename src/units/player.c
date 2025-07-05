#include "player.h"
#include "unit.h"
#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

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
  position.max_rotate_x = 10.0f;
  position.max_rotate_z = 15.0f;
  position.max_rotate_y = 0;
  position.rotate_x = 0.0f;
  position.rotate_y = 0.0f;
  position.rotate_z = 0.0f;
  position.max_angle = 15.0f;
  position.angle = 0.0f;
  return position;
};

PlayerRender newPlayerRender(PlayerPosition position) {
  PlayerRender render;
  render.position = position;
  render.size = newUnitSize();
  render.last_key_press = GetTime();
  render.acceleration = 0;
  render.direction_x_key = 0;
  render.direction_z_key = 0;
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
  player->render =
      newPlayerRender(newPlayerPosition(max_x, max_y, max_z, offset_z));
  player->model = model;
  return player;
}

void updatePlayer(Player *player) {
  if (!(IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_UP) ||
        IsKeyDown(KEY_DOWN))) {
    return;
  }
  double current_time = GetTime();
  double elapsed = current_time - player->render.last_key_press;
  player->render.last_key_press = current_time;
  bool drop_acceleration = elapsed > 0.2;
  if (IsKeyDown(KEY_LEFT) && player->render.direction_x_key != KEY_LEFT ||
      IsKeyDown(KEY_RIGHT) && player->render.direction_x_key != KEY_RIGHT ||
      IsKeyDown(KEY_UP) && player->render.direction_z_key != KEY_UP ||
      IsKeyDown(KEY_DOWN) && player->render.direction_z_key != KEY_DOWN) {
    drop_acceleration = true;
  }
  if (drop_acceleration) {
    player->render.acceleration = 0.1f;
  } else {
    player->render.acceleration += 0.05f;
  }
  if (player->render.acceleration > 1.0f) {
    player->render.acceleration = 1.0f;
  }
  float speed = 0.1f;

  if (IsKeyDown(KEY_LEFT)) {
    player->render.position.x -= player->render.acceleration;
    player->render.direction_x_key = KEY_LEFT;
  }
  if (IsKeyDown(KEY_RIGHT)) {
    player->render.position.x += player->render.acceleration;
    player->render.direction_x_key = KEY_RIGHT;
  }
  if (IsKeyDown(KEY_UP)) {
    player->render.position.z -= player->render.acceleration;
    player->render.direction_z_key = KEY_UP;
  }
  if (IsKeyDown(KEY_DOWN)) {
    player->render.position.z += player->render.acceleration;
    player->render.direction_z_key = KEY_DOWN;
  }
}
// void Player_events(Player *player, SDL_Keycode sym) {
//   uint32_t current_time = SDL_GetTicks();
//   uint32_t diff_time = player->render.last_key_press > current_time
//                            ? 0
//                            : current_time - player->render.last_key_press;
//   player->render.last_key_press = current_time;
//   if (diff_time > 200) {
//     player->render.acceleration = 1;
//   } else {
//     player->render.acceleration += 1;
//   }
//   if (player->render.acceleration > 10) {
//     player->render.acceleration = 10;
//   }
//   switch (sym) {
//   case SDLK_LEFT:
//     player->render.position.x -= player->render.acceleration;
//     break;
//   case SDLK_RIGHT:
//     player->render.position.x += player->render.acceleration;
//     break;
//   case SDLK_UP:
//     player->render.position.y -= player->render.acceleration;
//     break;
//   case SDLK_DOWN:
//     player->render.position.y += player->render.acceleration;
//     break;
//   }
// }

void drawPlayer(Player *player) {
  if (!player) {
    return;
  }

  DrawModelEx(
      player->model->model,
      (Vector3){player->render.position.x, player->render.position.y,
                player->render.position.z + player->render.position.offset_z},
      (Vector3){0, 1, 0}, // ось Y
      180.0f, (Vector3){1, 1, 1}, WHITE);
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
