#include "explosion.h"
#include "raylib.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define EXPLOSION_A "assets/textures/explosion_a.png"
#define EXPLOSION_A_NUM_FRAMES_PER_LINE 5
#define EXPLOSION_A_NUM_LINES 5

/**
 * @brief Creates and initializes a new explosion animation state.
 *
 * @param model Pointer to the ExplosionModel to be used for animation.
 * @return Pointer to a newly allocated ExplosionState, or NULL on failure.
 */
ExplosionState *newExplosionState(ExplosionModel *model) {
  ExplosionState *state = malloc(sizeof(ExplosionState));
  if (!state) {
    return NULL;
  }
  state->counter = 0;
  state->frame = 0;
  state->line = 0;
  state->model = model;
  state->active = true;
  return state;
}

/**
 * @brief Frees the memory associated with an explosion state.
 *
 * @param state Pointer to the ExplosionState to destroy.
 */
void destroyExplosionState(ExplosionState *state) {
  if (!state)
    return;
  free(state);
}

/**
 * @brief Draws the explosion animation as a billboard in 3D space.
 *
 * Advances the animation based on a simple frame counter and renders the
 * current frame using DrawBillboardRec.
 *
 * @param state Pointer to the active ExplosionState.
 * @param camera The 3D camera used to orient the billboard.
 * @param position The world position where the explosion should be rendered.
 */
void drawExplosionState(ExplosionState *state, Camera3D camera,
                        Vector3 position) {
  if (!state->active)
    return;

  state->counter++;
  if (state->counter > 2) {
    state->frame++;
    if (state->frame >= state->model->frames_per_line) {
      state->frame = 0;
      state->line++;
      if (state->line >= state->model->num_lines) {
        state->line = 0;
        state->active = false;
      }
    }
    state->counter = 0;
  }

  if (!state->active)
    return;

  Rectangle frame_rec = {state->model->frame_width * state->frame,
                         state->model->frame_height * state->line,
                         state->model->frame_width, state->model->frame_height};

  float aspect =
      (float)state->model->frame_width / (float)state->model->frame_height;

  float targetHeight = 20.0f;
  float targetWidth = targetHeight * aspect;
  Vector2 size = {targetWidth, targetHeight};

  DrawBillboardRec(camera, state->model->texture, frame_rec, position, size,
                   WHITE);
}

/**
 * @brief Creates a new node containing a single explosion model.
 *
 * Loads the sprite sheet and initializes frame metadata.
 *
 * @param path Path to the image file.
 * @param frames_per_line Number of frames per horizontal line in the sprite
 * sheet.
 * @param num_lines Number of lines (rows) in the sprite sheet.
 * @param prev Pointer to the previous node in the list.
 * @return Pointer to the newly allocated node, or NULL on failure.
 */
ExplosionModelNode *newExplosionModelNode(const char *path,
                                          uint16_t frames_per_line,
                                          uint16_t num_lines,
                                          ExplosionModelNode *prev) {
  ExplosionModelNode *node = malloc(sizeof(ExplosionModelNode));
  if (!node) {
    return NULL;
  }

  ExplosionModel model;
  Texture2D texture = LoadTexture(path);
  model.frame_width = (float)(texture.width / frames_per_line);
  model.frame_height = (float)(texture.height / num_lines);
  model.frames_per_line = frames_per_line;
  model.num_lines = num_lines;
  model.texture = texture;

  node->prev = prev;
  node->next = NULL;
  node->self = model;
  return node;
}

/**
 * @brief Frees an explosion model node and unloads its texture.
 *
 * @param node Pointer to the node to destroy.
 */
void destroyExplosionModelNode(ExplosionModelNode *node) {
  if (!node) {
    return;
  }
  UnloadTexture(node->self.texture);
  free(node);
}

/**
 * @brief Loads a predefined list of explosion models.
 *
 * Currently loads one hardcoded model (EXPLOSION_A), but can be extended.
 *
 * @return Pointer to a newly allocated ExplosionModelList, or NULL on failure.
 */
ExplosionModelList *loadExplosionModelList() {
  ExplosionModelList *models = malloc(sizeof(ExplosionModelList));
  if (!models) {
    return NULL;
  }
  models->length = 0;
  models->head = NULL;
  models->tail = NULL;
  const char *list[] = {EXPLOSION_A, NULL};
  const uint16_t pers[] = {EXPLOSION_A_NUM_FRAMES_PER_LINE};
  const uint16_t nums[] = {EXPLOSION_A_NUM_LINES};

  for (int i = 0; list[i] != NULL; i++) {
    printf("[Explosion] Loading model %s\n", list[i]);
    ExplosionModelNode *node =
        newExplosionModelNode(list[i], pers[i], nums[i], models->tail);
    if (!node) {
      destroyExplosionModelList(models);
      return NULL;
    }
    if (!models->head) {
      models->head = node;
    }
    if (models->tail) {
      models->tail->next = node;
    }
    models->tail = node;
    models->length += 1;
    printf("[Explosion]Model %s has been loaded\n", list[i]);
  }
  return models;
}

/**
 * @brief Frees all explosion model nodes and destroys the list.
 *
 * @param models Pointer to the ExplosionModelList to destroy.
 */
void destroyExplosionModelList(ExplosionModelList *models) {
  if (!models) {
    return;
  }
  ExplosionModelNode *node = models->head;
  while (node) {
    ExplosionModelNode *next = node->next;
    destroyExplosionModelNode(node);
    node = next;
  }
  models->head = models->tail = NULL;
  models->length = 0;
  free(models);
}