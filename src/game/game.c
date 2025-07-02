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
  UnitList *units = newUnitList(20, width, models->head->self);
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
  Camera3D camera = {.position = (Vector3){0.0f, 40.0f, 0.01f},
                     .target = (Vector3){0.0f, 0.0f, 0.0f},
                     .up = (Vector3){0.0f, 0.0f, -1.0f},
                     .fovy = 45.0f,
                     .projection = CAMERA_PERSPECTIVE};
  game->camera = camera;
  Light light = CreateLight(LIGHT_DIRECTIONAL, (Vector3){2, 4, 4},
                            (Vector3){0, -1, 0}, WHITE, game->models->shader);
  game->light = light;
  // float ambient[4] = {0.3f, 0.3f, 0.3f, 1.0f};
  // for (int i = 0; i < game->units->head->self.model->model.materialCount;
  // i++) {
  //   int loc = GetShaderLocation(
  //       game->units->head->self.model->model.materials[i].shader, "ambient");
  //   SetShaderValue(game->units->head->self.model->model.materials[i].shader,
  //                  loc, ambient, SHADER_UNIFORM_VEC4);
  // }
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
    Matrix *units = getMatrixFromUnitList(game->units);
    if (!game->units->head) {
      printf("[game] ooops\n");
      return;
    }
    Model *model = &game->units->head->self.model->model;
    BeginDrawing();
    ClearBackground(BLACK);

    BeginMode3D(game->camera);

    // DrawGrid(20, 1.0f);
    DrawMeshInstanced(model->meshes[0], model->materials[0], units,
                      (int)game->units->length);
    EndMode3D();

    EndDrawing();

    free(units);
  }
  printf("[game] finished\n");
}