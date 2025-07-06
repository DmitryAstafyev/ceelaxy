#ifndef EXPLOSION_H
#define EXPLOSION_H

#include "raylib.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Represents a sprite sheet used for explosion animation.
 */
typedef struct {
  Texture2D texture;   /**< Sprite sheet texture. */
  float frame_width;   /**< Width of a single frame in pixels. */
  float frame_height;  /**< Height of a single frame in pixels. */
  int frames_per_line; /**< Number of frames in each horizontal line. */
  int num_lines;       /**< Total number of lines (rows) in the sheet. */
} ExplosionModel;

/**
 * @brief Wrapper for a pointer to an explosion model.
 */
typedef struct {
  ExplosionModel *model; /**< Pointer to the associated explosion model. */
} ExplosionInstance;

/**
 * @brief Node in a doubly linked list containing an explosion model.
 */
typedef struct ExplosionModelNode {
  struct ExplosionModelNode *next; /**< Pointer to the next node. */
  struct ExplosionModelNode *prev; /**< Pointer to the previous node. */
  ExplosionModel self;             /**< Explosion model stored in this node. */
} ExplosionModelNode;

/**
 * @brief Doubly linked list of explosion models.
 */
typedef struct {
  ExplosionModelNode *head; /**< Pointer to the first node in the list. */
  ExplosionModelNode *tail; /**< Pointer to the last node in the list. */
  uint16_t length;          /**< Total number of nodes in the list. */
} ExplosionModelList;

/**
 * @brief Represents the animation state of an explosion instance.
 */
typedef struct {
  int frame;   /**< Current animation frame (column index). */
  int line;    /**< Current animation line (row index). */
  int counter; /**< Frame delay counter for controlling animation speed. */
  bool active; /**< Whether the explosion is currently active. */
  ExplosionModel *model; /**< Pointer to the model used for animation. */
} ExplosionState;

/**
 * @brief Creates and initializes a new explosion animation state.
 *
 * @param model Pointer to the explosion model used for animation.
 * @return Pointer to a newly allocated ExplosionState, or NULL on failure.
 */
ExplosionState *newExplosionState(ExplosionModel *model);

/**
 * @brief Destroys and frees the memory used by an explosion animation state.
 *
 * @param state Pointer to the ExplosionState to be destroyed.
 */
void destroyExplosionState(ExplosionState *state);

/**
 * @brief Draws the current frame of the explosion as a 3D billboard.
 *
 * @param state Pointer to the active ExplosionState.
 * @param camera The active 3D camera.
 * @param position 3D world position where the explosion should be rendered.
 */
void drawExplosionState(ExplosionState *state, Camera3D camera,
                        Vector3 position);

/**
 * @brief Creates a new explosion model node from a sprite sheet file.
 *
 * @param path Path to the image file.
 * @param frames_per_line Number of frames in each row of the sprite sheet.
 * @param num_lines Number of lines (rows) in the sprite sheet.
 * @param prev Pointer to the previous node in the list (may be NULL).
 * @return Pointer to the newly created ExplosionModelNode, or NULL on failure.
 */
ExplosionModelNode *newExplosionModelNode(const char *path,
                                          uint16_t frames_per_line,
                                          uint16_t num_lines,
                                          ExplosionModelNode *prev);

/**
 * @brief Destroys a model node and unloads its associated texture.
 *
 * @param node Pointer to the ExplosionModelNode to be destroyed.
 */
void destroyExplosionModelNode(ExplosionModelNode *node);

/**
 * @brief Loads a predefined list of explosion models.
 *
 * @return Pointer to a newly allocated ExplosionModelList, or NULL on failure.
 */
ExplosionModelList *loadExplosionModelList();

/**
 * @brief Destroys a model list and all its contained nodes and textures.
 *
 * @param models Pointer to the ExplosionModelList to destroy.
 */
void destroyExplosionModelList(ExplosionModelList *models);

#endif