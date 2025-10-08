#ifndef SPRITES_H
#define SPRITES_H

#include "raylib.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Represents a sprite sheet.
 */
typedef struct {
  Texture2D texture;   /**< Sprite sheet texture. */
  float frame_width;   /**< Width of a single frame in pixels. */
  float frame_height;  /**< Height of a single frame in pixels. */
  int frames_per_line; /**< Number of frames in each horizontal line. */
  int num_lines;       /**< Total number of lines (rows) in the sheet. */
} SpriteSheet;

/**
 * @brief Wrapper for a pointer to an explosion model.
 */
typedef struct {
  SpriteSheet *model; /**< Pointer to the associated explosion model. */
} SpriteSheetInstance;

/**
 * @brief Node in a doubly linked list containing an explosion model.
 */
typedef struct SpriteSheetNode {
  struct SpriteSheetNode *next; /**< Pointer to the next node. */
  struct SpriteSheetNode *prev; /**< Pointer to the previous node. */
  SpriteSheet self;             /**< Explosion model stored in this node. */
} SpriteSheetNode;

/**
 * @brief Doubly linked list of explosion models.
 */
typedef struct {
  SpriteSheetNode *head; /**< Pointer to the first node in the list. */
  SpriteSheetNode *tail; /**< Pointer to the last node in the list. */
  uint16_t length;       /**< Total number of nodes in the list. */
} SpriteSheetList;

/**
 * @brief Represents the animation state of an explosion instance.
 */
typedef struct {
  int frame;   /**< Current animation frame (column index). */
  int line;    /**< Current animation line (row index). */
  int counter; /**< Frame delay counter for controlling animation speed. */
  bool active; /**< Whether the explosion is currently active. */
  int repeats;
  float opacity;
  float size;
  SpriteSheet *model; /**< Pointer to the model used for animation. */
} SpriteSheetState;

/**
 * @brief Creates and initializes a new explosion animation state.
 *
 * @param model Pointer to the explosion model used for animation.
 * @return Pointer to a newly allocated SpriteSheetState, or NULL on failure.
 */
SpriteSheetState *newSpriteSheetState(SpriteSheet *model, int repeats,
                                      float size, float opacity);

/**
 * @brief Destroys and frees the memory used by an explosion animation state.
 *
 * @param state Pointer to the SpriteSheetState to be destroyed.
 */
void destroySpriteSheetState(SpriteSheetState *state);

void dropSpriteSheetState(SpriteSheetState *state);
/**
 * @brief Draws the current frame of the explosion as a 3D billboard.
 *
 * @param state Pointer to the active SpriteSheetState.
 * @param camera The active 3D camera.
 * @param position 3D world position where the explosion should be rendered.
 */
void drawSpriteSheetState(SpriteSheetState *state, Camera3D camera,
                          Vector3 position);

/**
 * @brief Creates a new explosion model node from a sprite sheet file.
 *
 * @param path Path to the image file.
 * @param frames_per_line Number of frames in each row of the sprite sheet.
 * @param num_lines Number of lines (rows) in the sprite sheet.
 * @param prev Pointer to the previous node in the list (may be NULL).
 * @return Pointer to the newly created SpriteSheetNode, or NULL on failure.
 */
SpriteSheetNode *newSpriteSheetNode(const char *path, uint16_t frames_per_line,
                                    uint16_t num_lines, SpriteSheetNode *prev);

/**
 * @brief Destroys a model node and unloads its associated texture.
 *
 * @param node Pointer to the SpriteSheetNode to be destroyed.
 */
void destroySpriteSheetNode(SpriteSheetNode *node);

/**
 * @brief Loads a predefined list of explosion models.
 *
 * @return Pointer to a newly allocated SpriteSheetList, or NULL on failure.
 */
SpriteSheetList *loadSpriteSheetList();

/**
 * @brief Destroys a model list and all its contained nodes and textures.
 *
 * @param models Pointer to the SpriteSheetList to destroy.
 */
void destroySpriteSheetList(SpriteSheetList *models);

SpriteSheet newSpriteSheet(const char *path, uint16_t frames_per_line,
                           uint16_t num_lines);

#endif
