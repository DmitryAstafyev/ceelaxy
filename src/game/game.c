/**
 * @file game.c
 * @brief Implements the core game logic, including initialization, main loop,
 * rendering, and resource cleanup.
 */
#include "game.h"
#include "../bullets/bullets.h"
#include "../models/models.h"
#include "../raylib/rlights.h"
#include "../sprites/sprites.h"
#include "../textures/textures.h"
#include "../units/bars.h"
#include "../units/player.h"
#include "../units/unit.h"
#include "../utils/debug.h"
#include "levels.h"
#include "raylib.h"
#include "stat.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
  game->stat = newGameStat();
  // Load textures
  GameTextures *textures = createGameTexturesList();
  if (!textures) {
    return NULL;
  }
  // Load models
  ShipModelList *models = newShipModelList();
  if (!models || models->length == 0) {
    destroyTexturesList(textures);
    return NULL;
  }
  ShipModel *enemy_model = findModelInList(models, MODEL_CAMO_STELLAR_JET);
  if (!enemy_model) {
    destroyTexturesList(textures);
    return NULL;
  }
  // Create bullets storage
  BulletList *bullets = newBulletList();
  if (!bullets) {
    destroyTexturesList(textures);
    return NULL;
  }

  UnitList *enemies = newUnitList(20, enemy_model, 10, 3, 40.0f, textures);
  if (!enemies) {
    destroyBulletList(bullets);
    destroyTexturesList(textures);
    return NULL;
  }
  // Load explosions
  SpriteSheetList *sprites = loadSpriteSheetList();
  if (!sprites) {
    destroyBulletList(bullets);
    destroyTexturesList(textures);
    destroyUnitList(enemies);
    return NULL;
  }
  // Create a player
  ShipModel *player_model = findModelInList(models, MODEL_TRANSTELLAR);
  if (!player_model) {
    destroyBulletList(bullets);
    destroySpriteSheetList(sprites);
    destroyTexturesList(textures);
    destroyUnitList(enemies);
    return NULL;
  }
  Player *player =
      newPlayer(20.0f, 0.0f, 20.0f, 30.0f, player_model, bullets, textures);
  if (!player) {
    destroySpriteSheetList(sprites);
    destroyTexturesList(textures);
    destroyUnitList(enemies);
    destroyBulletList(bullets);
    return NULL;
  }
  game->bullets = bullets;
  game->player = player;
  game->models = models;
  game->enemies = enemies;
  game->sprites = sprites;
  game->textures = textures;
  game->level = getFirstLevel();
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
  destroySpriteSheetList(game->sprites);
  destroyTexturesList(game->textures);
  free(game);
}

bool nextGameLevel(Game *game) {
  game->level = goToNextLevel(game->level);
  ShipModel *enemy_model =
      findModelInListCycle(game->models, game->level.level);
  if (!enemy_model) {
    return false;
  }
  UnitList *enemies =
      newUnitList(20, enemy_model, 10, 3, 40.0f, game->textures);
  if (!enemies) {
    return false;
  }
  game->enemies = enemies;
  game->player->state.health = 100;
  game->player->state.energy = 100;
  return true;
}

void dropGameLevel(Game *game) {
  game->level = getFirstLevel();
  ShipModel *enemy_model =
      findModelInList(game->models, MODEL_CAMO_STELLAR_JET);
  if (!enemy_model) {
    return;
  }
  UnitList *enemies =
      newUnitList(20, enemy_model, 10, 3, 40.0f, game->textures);
  if (!enemies) {
    return;
  }
  game->enemies = enemies;
  game->player->state.health = 100;
  game->player->state.energy = 100;
  game->stat = newGameStat();
}

void gameOverDraw() {

  const char *text = TextFormat("Game Over");
  int font = LEVEL_LABEL_FONT_SIZE;

  int textW = MeasureText(text, font);
  int textH = font;
  int x = (GetScreenWidth() - textW) / 2;
  int y = (GetScreenHeight() - textH) / 2;

  DrawText(text, x + 2, y + 2, font, Fade(BLACK, 0.5f));
  DrawText(text, x, y, font, Fade(RAYWHITE, 1.0f));
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
  double over_tm = GetTime();
  bool over = false;
  while (!WindowShouldClose()) {
    if (game->player->state.health <= 0 && !over) {
      over_tm = GetTime();
      over = true;
    }
    if (over) {
      if (GetTime() - over_tm > 5.0) {
        over = false;
        dropGameLevel(game);
      }
    }
    if (!game->enemies->head) {
      printf("[game] next level!\n");
      if (!nextGameLevel(game)) {
        return;
      }
    }
    BeginDrawing();
    ClearBackground(BLACK);

    BeginMode3D(game->camera);

    if (is_debug_mode) {
      DrawCube((Vector3){0.0f, 0.0f, 0.0f}, 1.0f, 1.0f, 1.0f, RED);
    }
    checkBulletHitsUnits(game->enemies, game->bullets, &game->stat);
    checkBulletHitsPlayer(game->player, game->bullets, &game->stat);
    drawUnits(game->enemies, &game->camera, game->sprites);
    selectUnitsToFire(game->enemies, &game->camera, game->player, &game->level,
                      10.0, game->textures);
    if (!over) {
      drawPlayer(game->player, &game->level, game->textures, &game->camera,
                 game->sprites);
      drawBullets(game->bullets, &game->camera, &game->stat);
    }

    EndMode3D();

    if (!over) {
      drawPlayerStateBars(game->player, &game->camera);
    }

    drawUnitsStateBars(game->enemies, &game->camera);
    gameStatDraw(&game->stat);
    levelDraw(&game->level);
    if (over) {
      gameOverDraw();
    }
    EndDrawing();
  }
  printf("[game] finished\n");
}