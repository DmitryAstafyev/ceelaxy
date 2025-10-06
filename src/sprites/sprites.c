#include "sprites.h"
#include "raylib.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

SpriteSheet newSpriteSheet(const char *path, uint16_t frames_per_line,
                           uint16_t num_lines) {
  Texture2D texture = LoadTexture(path);
  if (texture.id == 0) {
    TraceLog(LOG_ERROR, "Fail to load texture: %s", path);
    exit(1);
  }
  SpriteSheet model;
  model.frame_width = (float)texture.width / frames_per_line;
  model.frame_height = (float)texture.height / num_lines;
  model.frames_per_line = frames_per_line;
  model.num_lines = num_lines;
  model.texture = texture;
  return model;
}

#define EXPLOSION_A "assets/textures/explosion_a.png"
#define EXPLOSION_A_NUM_FRAMES_PER_LINE 5
#define EXPLOSION_A_NUM_LINES 5

#define EXPLOSION_B "assets/textures/explosion_b.png"
#define EXPLOSION_B_NUM_FRAMES_PER_LINE 3
#define EXPLOSION_B_NUM_LINES 3

#define SMOKE_A "assets/textures/smoke_a.png"
#define SMOKE_A_NUM_FRAMES_PER_LINE 5
#define SMOKE_A_NUM_LINES 3

/**
 * @brief Creates and initializes a new sprite animation state.
 *
 * @param model Pointer to the SpriteSheet to be used for animation.
 * @return Pointer to a newly allocated SpriteSheetState, or NULL on failure.
 */
SpriteSheetState *newSpriteSheetState(SpriteSheet *model, int repeats,
                                      float size, float opacity) {
  SpriteSheetState *state = malloc(sizeof(SpriteSheetState));
  if (!state) {
    return NULL;
  }
  state->counter = 0;
  state->frame = 0;
  state->line = 0;
  state->model = model;
  state->active = true;
  state->repeats = repeats;
  state->opacity = opacity;
  state->size = size;
  return state;
}

/**
 * @brief Frees the memory associated with an sprite state.
 *
 * @param state Pointer to the SpriteSheetState to destroy.
 */
void destroySpriteSheetState(SpriteSheetState *state) {
  if (!state)
    return;
  free(state);
}

void dropSpriteSheetState(SpriteSheetState *state) {
  if (!state || state->active)
    return;
  state->active = true;
  state->counter = 0;
  state->frame = 0;
  state->line = 0;
}
/**
 * @brief Draws the sprite animation as a billboard in 3D space.
 *
 * Advances the animation based on a simple frame counter and renders the
 * current frame using DrawBillboardRec.
 *
 * @param state Pointer to the active SpriteSheetState.
 * @param camera The 3D camera used to orient the billboard.
 * @param position The world position where the sprite should be rendered.
 */
void drawSpriteSheetState(SpriteSheetState *state, Camera3D camera,
                          Vector3 position) {
  if (!state || !state->active)
    return;

  state->frame++;
  if (state->frame >= state->model->frames_per_line) {
    state->frame = 0;
    state->line++;
    if (state->line >= state->model->num_lines) {
      state->line = 0;
      state->counter++;
    }
  }
  if (state->counter > state->repeats) {
    state->active = false;
  }
  if (!state->active)
    return;

  Rectangle frame_rec = {state->model->frame_width * state->frame,
                         state->model->frame_height * state->line,
                         state->model->frame_width, state->model->frame_height};

  float aspect =
      (float)state->model->frame_width / (float)state->model->frame_height;

  float targetWidth = state->size * aspect;
  Vector2 size = {targetWidth, state->size};

  DrawBillboardRec(camera, state->model->texture, frame_rec, position, size,
                   WHITE);
}

/**
 * @brief Creates a new node containing a single sprite model.
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
SpriteSheetNode *newSpriteSheetNode(const char *path, uint16_t frames_per_line,
                                    uint16_t num_lines, SpriteSheetNode *prev) {
  SpriteSheetNode *node = malloc(sizeof(SpriteSheetNode));
  if (!node) {
    return NULL;
  }

  SpriteSheet model = newSpriteSheet(path, frames_per_line, num_lines);

  node->prev = prev;
  node->next = NULL;
  node->self = model;
  return node;
}

/**
 * @brief Frees an sprite model node and unloads its texture.
 *
 * @param node Pointer to the node to destroy.
 */
void destroySpriteSheetNode(SpriteSheetNode *node) {
  if (!node) {
    return;
  }
  UnloadTexture(node->self.texture);
  free(node);
}

/**
 * @brief Loads a predefined list of sprite models.
 *
 * Currently loads one hardcoded model (EXPLOSION_A), but can be extended.
 *
 * @return Pointer to a newly allocated SpriteSheetList, or NULL on failure.
 */
SpriteSheetList *loadSpriteSheetList() {
  SpriteSheetList *models = malloc(sizeof(SpriteSheetList));
  if (!models) {
    return NULL;
  }
  models->length = 0;
  models->head = NULL;
  models->tail = NULL;
  // const char *list[] = {EXPLOSION_A, EXPLOSION_B, SMOKE_A, NULL};
  // const uint16_t pers[] = {EXPLOSION_A_NUM_FRAMES_PER_LINE,
  //                          EXPLOSION_B_NUM_FRAMES_PER_LINE,
  //                          SMOKE_A_NUM_FRAMES_PER_LINE};
  // const uint16_t nums[] = {EXPLOSION_A_NUM_LINES, EXPLOSION_B_NUM_LINES,
  //                          SMOKE_A_NUM_LINES};
  const char *list[] = {EXPLOSION_A, EXPLOSION_B, NULL};
  const uint16_t pers[] = {
      EXPLOSION_A_NUM_FRAMES_PER_LINE,
      EXPLOSION_B_NUM_FRAMES_PER_LINE,
  };
  const uint16_t nums[] = {
      EXPLOSION_A_NUM_LINES,
      EXPLOSION_B_NUM_LINES,
  };

  for (int i = 0; list[i] != NULL; i++) {
    printf("[Explosion] Loading model %s\n", list[i]);
    SpriteSheetNode *node =
        newSpriteSheetNode(list[i], pers[i], nums[i], models->tail);
    if (!node) {
      destroySpriteSheetList(models);
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
 * @brief Frees all sprite model nodes and destroys the list.
 *
 * @param models Pointer to the SpriteSheetList to destroy.
 */
void destroySpriteSheetList(SpriteSheetList *models) {
  if (!models) {
    return;
  }
  SpriteSheetNode *node = models->head;
  while (node) {
    SpriteSheetNode *next = node->next;
    destroySpriteSheetNode(node);
    node = next;
  }
  models->head = models->tail = NULL;
  models->length = 0;
  free(models);
}