#include "player.h"
#include "unit.h"
#include <raylib.h>
#include <raymath.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

PlayerRender newPlayerRender(UnitPosition position, int height, int width) {
  PlayerRender render;
  render.position = position;
  render.size = newUnitSize();
  UnitSize window;
  window.height = height;
  window.width = width;
  render.window = window;
  render.last_key_press = 0;
  render.acceleration = 0;
  return render;
}

Player *newPlayer(int height, int width) {
  Player *player = malloc(sizeof(Player));
  if (!player) {
    return NULL;
  }
  player->type = UNIT_TYPE_SOLDER;
  player->state = newUnitState();
  player->render = newPlayerRender(newUnitPosition(), height, width);
  player->render.position.x = width / 2 - player->render.size.width / 2;
  player->render.position.y = height - player->render.size.height;
  return player;
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

void destroyPlayer(Player *player) {
  if (!player) {
    return;
  }
  free(player);
}

Matrix getPlayerMatrix(Player *player) {
  return MatrixTranslate(player->render.position.x, player->render.position.y,
                         player->render.position.z);
}
