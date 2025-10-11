#include "textures.h"
#include "raylib.h"
#include "rlgl.h"
#include <stdbool.h>
#include <stdlib.h>

#define TEX_PATH_FIRE_SOFT "assets/textures/fire_soft.png"
#define TEX_PATH_FIRE_STREAK "assets/textures/fire_streak.png"
#define TEX_PATH_GLOW "assets/textures/glow.png"
#define TEX_PATH_SMOKE_SOFT "assets/textures/smoke_soft.png"

// Array of texture file paths corresponding to their IDs
const char *TEX_PATHS[] = {TEX_PATH_FIRE_SOFT, TEX_PATH_FIRE_STREAK,
                           TEX_PATH_GLOW, TEX_PATH_SMOKE_SOFT};

/**
 * @brief Adds a new texture to the GameTextures list.
 *
 * Loads the texture from the specified path and assigns the given ID.
 *
 * @param list Pointer to the GameTextures list.
 * @param path Path to the texture file.
 * @param id Unique identifier for the texture.
 * @return true if the texture was added successfully, false otherwise.
 */
bool addGameTextureIntoList(GameTextures *list, const char *path, int id)
{
  if (!list || !path)
  {
    return false;
  }

  Image img = LoadImage(path);
  if (!img.data)
  {
    TraceLog(LOG_ERROR, "Failed to load image: %s", path);
    return false;
  }

  GameTexture *node = malloc(sizeof(GameTexture));
  node->id = id;
  if (!node)
  {
    UnloadImage(img);
    TraceLog(LOG_ERROR, "Failed to allocate memory for: %s", path);
    return false;
  }

  node->tex = LoadTextureFromImage(img);
  UnloadImage(img);

  if (node->tex.id == 0)
  {
    free(node);
    TraceLog(LOG_ERROR, "Failed to create texture from image: %s", path);
    return false;
  }

  node->next = NULL;
  node->prev = list->tail;

  if (list->tail)
  {
    list->tail->next = node;
  }
  else
  {
    list->home = node;
  }
  list->tail = node;
  TraceLog(LOG_INFO, "Texture is loaded: %s", path);

  return true;
}

/**
 * @brief Frees a GameTexture node and unloads its texture.
 *
 * @param tex Pointer to the GameTexture node to destroy.
 */
void destroyGameTexture(GameTexture *tex)
{
  if (!tex)
  {
    return;
  }
  if (tex->tex.id != 0)
  {
    UnloadTexture(tex->tex);
  }
  free(tex);
}

/**
 * @brief Frees all textures and nodes in the GameTextures list.
 *
 * @param list Pointer to the GameTextures list to destroy.
 */
void destroyTexturesList(GameTextures *list)
{
  if (!list)
  {
    return;
  }
  GameTexture *cur = list->home;
  while (cur)
  {
    GameTexture *next = cur->next;
    destroyGameTexture(cur);
    cur = next;
  }
  free(list);
}

/**
 * @brief Creates and initializes a new GameTextures list.
 *
 * @return Pointer to the newly created GameTextures list, or NULL on failure.
 */
GameTextures *createGameTexturesList(void)
{
  GameTextures *list = malloc(sizeof(GameTextures));
  if (!list)
    return NULL;
  list->home = NULL;
  list->tail = NULL;

  for (unsigned int i = 0; i < (int)(sizeof(TEX_PATHS) / sizeof(TEX_PATHS[0]));
       ++i)
  {
    if (!addGameTextureIntoList(list, TEX_PATHS[i], i))
    {
      destroyTexturesList(list);
      return NULL;
    }
  }
  return list;
}

/**
 * @brief Retrieves a texture node by its unique ID.
 *
 * @param list Pointer to the GameTextures list.
 * @param id The unique identifier of the texture to retrieve.
 * @return Pointer to the GameTexture node with the specified ID, or NULL if not found.
 */
GameTexture *getGameTextureById(GameTextures *list, unsigned int id)
{
  if (list == NULL)
  {
    return NULL;
  }
  for (GameTexture *node = list->home; node; node = node->next)
  {
    if (node->id == id)
      return node;
  }
  return NULL;
}
