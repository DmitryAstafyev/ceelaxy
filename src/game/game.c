#include "game.h"
#include "../models/models.h"
#include "../raylib/rlights.h"
#include "../units/player.h"
#include "../units/unit.h"
#include "raylib.h" // For Model, Texture2D, Shader
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

Game *newGame(int height, int width) {
  Game *game = malloc(sizeof(Game));
  if (!game) {
    return NULL;
  }
  // Load models
  ShipModelList *models = newShipModelList();
  if (!models || models->length == 0) {
    return NULL;
  }
  UnitList *units = newUnitList(20, models->head->self, 10, 3, 40.0f);
  if (!units) {
    return NULL;
  }
  Player *player = newPlayer(height, width);
  if (!player) {
    destroyUnitList(game->units);
    return NULL;
  }
  game->player = player;
  game->models = models;
  game->units = units;
  Camera3D camera = {.position = (Vector3){0.0f, 80.0f, 40.0f},
                     .target = (Vector3){0.0f, 0.0f, 0.0f},
                     .up = (Vector3){0.0f, 1.0f, 0.0f},
                     .fovy = 45.0f,
                     .projection = CAMERA_PERSPECTIVE};
  game->camera = camera;
  Light light = CreateLight(LIGHT_DIRECTIONAL, (Vector3){2, 4, 4},
                            (Vector3){0, -1, 0}, WHITE, game->models->shader);
  game->light = light;

  int ambientLoc = GetShaderLocation(game->models->shader, "ambient");
  float ambient[4] = {0.3f, 0.3f, 0.3f, 1.0f};
  SetShaderValue(game->models->shader, ambientLoc, ambient,
                 SHADER_UNIFORM_VEC4);

  printf("[game] Game has been created\n");
  return game;
}

bool renderGame(Game *game) {
  if (!game) {
    return false;
  }

  return true;
}

void destroyGame(Game *game) {
  if (!game) {
    return;
  }
  destroyUnitList(game->units);
  destroyPlayer(game->player);
  destroyShipModelList(game->models);
  free(game);
}

void runGame(Game *game) {
  printf("[game] starting\n");
  while (!WindowShouldClose()) {
    if (!game->units->head) {
      printf("[game] ooops\n");
      return;
    }
    // Model *model = &game->units->head->self.model->model;
    BeginDrawing();
    ClearBackground(BLACK);

    BeginMode3D(game->camera);
    DrawCube((Vector3){0.0f, 0.0f, 0.0f}, 1.0f, 1.0f, 1.0f, RED);

    // DrawModelEx(*model, (Vector3){0.0f, 0.0f, 0.0f}, (Vector3){0, 0, 0},
    // 0.0f,
    //             (Vector3){1, 1, 1}, WHITE);
    drawUnits(game->units);
    EndMode3D();

    EndDrawing();
  }
  printf("[game] finished\n");
}