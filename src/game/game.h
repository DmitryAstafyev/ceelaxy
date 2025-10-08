/**
 * @file game.h
 * @brief Declares the main Game structure and high-level game lifecycle
 * functions, including creation, rendering, execution, and cleanup.
 */
#ifndef GAME_H
#define GAME_H

#include "../bullets/bullets.h"
#include "../models/models.h"
#include "../raylib/rlights.h"
#include "../sprites/sprites.h"
#include "../textures/textures.h"
#include "../units/player.h"
#include "../units/unit.h"
#include "raylib.h"
#include "stat.h"
#include <stdbool.h>

/**
 * @brief Central structure representing the full game state.
 *
 * Contains references to player, enemies, projectiles, models, and the 3D
 * rendering context.
 */
typedef struct Game {
  UnitList *enemies;   ///< Linked list of enemy units.
  Player *player;      ///< Pointer to the player instance.
  BulletList *bullets; ///< Shared bullet registry for both player and enemies.
  ShipModelList *models; ///< List of loaded 3D models used by the game.
  GameTextures *textures;
  SpriteSheetList *sprites; /// < List of loaded sprites textures/models
  Camera3D camera;          ///< Active 3D camera used for rendering the scene.
  Light light;              ///< Scene lighting setup for shading.
  GameStat stat;
} Game;

/**
 * @brief Allocates and initializes a new Game instance.
 *
 * Sets up player, enemies, models, bullets, camera, and lighting.
 *
 * @param height Screen or viewport height in pixels.
 * @param width Screen or viewport width in pixels.
 * @return Pointer to a fully initialized Game structure, or NULL on failure.
 */
Game *newGame(int height, int width);

/**
 * @brief Releases all memory and resources associated with the Game.
 *
 * Frees player, enemies, bullets, models, and any other allocated objects.
 *
 * @param game Pointer to the game instance to destroy.
 */
void destroyGame(Game *game);

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
void runGame(Game *game);

#endif