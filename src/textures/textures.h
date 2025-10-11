#pragma once

#include "raylib.h"
#include "rlgl.h"

static unsigned int TEX_ID_FIRE_SOFT = 0;
static unsigned int TEX_ID_FIRE_STREAK = 1;
static unsigned int TEX_ID_GLOW = 2;
static unsigned int TEX_ID_SMOKE_SOFT = 3;

/**
 * @brief Node in a doubly linked list containing a texture and its ID.
 */
typedef struct GameTexture
{
  struct GameTexture *next; /// Pointer to the next texture in the list.
  struct GameTexture *prev; /// Pointer to the previous texture in the list.
  Texture2D tex;            /// The actual texture data.
  int id;                   /// Unique identifier for the texture.
} GameTexture;

/**
 * @brief Doubly linked list of game textures.
 */
typedef struct GameTextures
{
  GameTexture *home; /// Pointer to the first texture in the list.
  GameTexture *tail; /// Pointer to the last texture in the list.
} GameTextures;

/**
 * @brief Creates and initializes a new GameTexture node.
 *
 * Loads the texture from the specified path and assigns the given ID.
 *
 * @param path Path to the texture file.
 * @param id Unique identifier for the texture.
 * @param prev Pointer to the previous GameTexture node (can be NULL).
 * @return Pointer to the newly created GameTexture node, or NULL on failure.
 */
void destroyTexturesList(GameTextures *list);

/**
 * @brief Creates and initializes a new GameTextures list.
 *
 * @return Pointer to the newly created GameTextures list, or NULL on failure.
 */
GameTextures *createGameTexturesList(void);

/**
 * @brief Frees all textures and nodes in the GameTextures list.
 *
 * @param list Pointer to the GameTextures list to destroy.
 */
GameTexture *getGameTextureById(GameTextures *list, unsigned int id);