/**
 * @file game.c
 * @brief Implements the core game logic, including initialization, main loop,
 * rendering, and resource cleanup.
 */
#include "game.h"
#include "../bullets/bullets.h"
#include "../models/models.h"
#include "../raylib/rlights.h"
#include "../units/explosion.h"
#include "../units/player.h"
#include "../units/unit.h"
#include "../utils/debug.h"
#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

/**
 * @brief Initializes a new Game instance, loading models, creating player,
 * enemies, and bullets.
 *
 * Also sets up the 3D camera and lighting. If any step fails, previously
 * allocated resources are freed and NULL is returned.
 *
 * @param height The height of the game window or viewport (currently unused).
 * @param width The width of the game window or viewport (currently unused).
 * @return Pointer to a fully initialized Game structure, or NULL on failure.
 */
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
  // Load explosions
  ExplosionModelList *explosions = loadExplosionModelList();
  if (!explosions) {
    destroyUnitList(enemies);
    return NULL;
  }
  // Create bullets storage
  BulletList *bullets = newBulletList();
  if (!bullets) {
    destroyUnitList(enemies);
    return NULL;
  }
  // Create a player
  ShipModel *player_model = findModelInList(models, MODEL_Transtellar);
  if (!player_model) {
    return NULL;
  }
  Player *player = newPlayer(20.0f, 0.0f, 20.0f, 30.0f, player_model, bullets);
  if (!player) {
    destroyUnitList(game->enemies);
    destroyBulletList(bullets);
    return NULL;
  }
  game->bullets = bullets;
  game->player = player;
  game->models = models;
  game->enemies = enemies;
  game->explosions = explosions;
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

/**
 * @brief Frees all memory and resources associated with a Game instance.
 *
 * Properly destroys the enemies, player, bullet list, and model list.
 *
 * @param game Pointer to the Game to destroy.
 */
void destroyGame(Game *game) {
  if (!game) {
    return;
  }
  destroyUnitList(game->enemies);
  destroyPlayer(game->player);
  destroyShipModelList(game->models);
  destroyBulletList(game->bullets);
  destroyExplosionModelList(game->explosions);
  free(game);
}

/**
 * @brief Runs the main game loop until the window is closed.
 *
 * The loop performs:
 * - Bullet hit checks
 * - Drawing of units, player, and bullets
 * - Handling debug rendering
 *
 * If the enemy list becomes empty, the loop exits early.
 *
 * @param game Pointer to the initialized Game instance.
 */
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
    if (is_debug_mode) {
      DrawCube((Vector3){0.0f, 0.0f, 0.0f}, 1.0f, 1.0f, 1.0f, RED);
    }
    checkBulletHitsUnits(game->enemies, game->bullets);
    drawUnits(game->enemies, &game->camera, game->explosions);
    drawPlayer(game->player);
    drawBullets(game->bullets);
    EndMode3D();

    EndDrawing();
  }
  printf("[game] finished\n");
}