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
  ShipModel *enemy_model = findModelInList(models, MODEL_CamoStellarJet);
  if (!enemy_model) {
    return NULL;
  }
  UnitList *enemies = newUnitList(20, enemy_model, 10, 3, 40.0f);
  if (!enemies) {
    return NULL;
  }
  ShipModel *player_model = findModelInList(models, MODEL_Transtellar);
  if (!player_model) {
    return NULL;
  }
  Player *player = newPlayer(20.0f, 0.0f, 20.0f, 30.0f, player_model);
  if (!player) {
    destroyUnitList(game->enemies);
    return NULL;
  }
  game->player = player;
  game->models = models;
  game->enemies = enemies;
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
  destroyUnitList(game->enemies);
  destroyPlayer(game->player);
  destroyShipModelList(game->models);
  free(game);
}

void runGame(Game *game) {
  printf("[game] starting\n");
  while (!WindowShouldClose()) {
    if (!game->enemies->head) {
      printf("[game] ooops\n");
      return;
    }
    BeginDrawing();
    ClearBackground(BLACK);

    BeginMode3D(game->camera);
    DrawCube((Vector3){0.0f, 0.0f, 0.0f}, 1.0f, 1.0f, 1.0f, RED);
    drawUnits(game->enemies);
    drawPlayer(game->player);
    updatePlayer(game->player);
    EndMode3D();

    EndDrawing();
  }
  printf("[game] finished\n");
}