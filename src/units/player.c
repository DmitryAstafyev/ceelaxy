/**
 * @file player.c
 * @brief Implements player initialization logic, including position, visual
 * state, movement parameters, and rendering configuration.
 */
#include "player.h"
#include "../bullets/bullets.h"
#include "../utils/debug.h"
#include "unit.h"
#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

/// Minimum delay (in seconds) between direction changes to reset acceleration.
static const float ACCELERATION_DEALY = 0.2f;

/// Initial acceleration value when direction is changed.
static const float ACCELERATION_INIT = 0.1f;

/// Acceleration increment when holding a direction key.
static const float ACCELERATION_STEP = 0.05f;

/// Maximum acceleration value allowed.
static const float ACCELERATION_MAX = 1.0f;

/// Maximum rotation angle along the X axis during movement.
static const float MAX_ROTATE_X = 15.0f;

/// Maximum rotation angle along the Y axis (unused).
static const float MAX_ROTATE_Y = 0.0f;

/// Maximum rotation angle along the Z axis during movement.
static const float MAX_ROTATE_Z = 35.0f;

/// Step of rotation applied per frame along the X axis.
static const float STEP_ROTATE_X = 1.0f;

/// Step of rotation along the Y axis (unused).
static const float STEP_ROTATE_Y = 0.0f;

/// Step of rotation applied per frame along the Z axis.
static const float STEP_ROTATE_Z = 2.0f;

/**
 * @brief Initializes a new player position with boundaries and Z offset.
 *
 * @param max_x Maximum allowed position on the X axis.
 * @param max_y Maximum allowed position on the Y axis.
 * @param max_z Maximum allowed position on the Z axis.
 * @param offset_z Offset applied along the Z axis for visual or logic
 * separation.
 * @return Initialized PlayerPosition struct with all positions set to 0 and
 * bounds applied.
 */
PlayerPosition newPlayerPosition(float max_x, float max_y, float max_z,
                                 float offset_z) {
  PlayerPosition position;
  position.x = 0.0f;
  position.y = 0.0f;
  position.z = 0.0f;
  position.max_x = max_x;
  position.max_y = max_y;
  position.max_z = max_z;
  position.offset_z = offset_z;
  return position;
};

/**
 * @brief Initializes the player's visual rotation state.
 *
 * Sets maximum rotation angles, initial angles, and step increments for dynamic
 * tilting in response to movement input.
 *
 * @param offset_z Z-axis offset, may be used for visual calibration.
 * @return Initialized PlayerVisualState struct with zero angles and defined
 * limits.
 */
PlayerVisualState newPlayerVisualState(float offset_z) {
  PlayerVisualState state;
  state.max_rotate_x = MAX_ROTATE_X;
  state.max_rotate_y = MAX_ROTATE_Y;
  state.max_rotate_z = MAX_ROTATE_Z;
  state.rotate_x = 0.0f;
  state.rotate_y = 0.0f;
  state.rotate_z = 0.0f;
  state.rotate_step_x = STEP_ROTATE_X;
  state.rotate_step_y = STEP_ROTATE_Y;
  state.rotate_step_z = STEP_ROTATE_Z;
  state.max_angle = 15.0f;
  state.angle = 0.0f;
  return state;
};

/**
 * @brief Initializes the player's visual rotation state.
 *
 * Sets maximum rotation angles, initial angles, and step increments for dynamic
 * tilting in response to movement input.
 *
 * @param offset_z Z-axis offset, may be used for visual calibration.
 * @return Initialized PlayerVisualState struct with zero angles and defined
 * limits.
 */
PlayerMovement newPlayerMovement() {
  PlayerMovement movement;
  movement.last_key_press = GetTime();
  movement.acceleration = 0;
  movement.direction_x_key = 0;
  movement.direction_z_key = 0;
  return movement;
}

/**
 * @brief Composes the rendering configuration of the player.
 *
 * Internally calls constructors for position, size, visual state,
 * and movement, effectively encapsulating all visual and positional
 * logic into a single PlayerRender struct.
 *
 * @param max_x Maximum X position allowed.
 * @param max_y Maximum Y position allowed.
 * @param max_z Maximum Z position allowed.
 * @param offset_z Offset to be applied on the Z axis.
 * @return Initialized PlayerRender struct with all required fields.
 */
PlayerRender newPlayerRender(float max_x, float max_y, float max_z,
                             float offset_z) {
  PlayerRender render;
  render.position = newPlayerPosition(max_x, max_y, max_z, offset_z);
  render.size = newUnitSize();
  render.state = newPlayerVisualState(offset_z);
  render.movement = newPlayerMovement();
  return render;
}

/**
 * @brief Creates a new Player instance with specified world bounds and
 * dependencies.
 *
 * Allocates memory and initializes the player object with movement and
 * rendering state. Returns NULL if memory allocation fails or the model pointer
 * is NULL.
 *
 * @param max_x Maximum X-axis boundary for player movement.
 * @param max_y Maximum Y-axis boundary (typically unused or static).
 * @param max_z Maximum Z-axis boundary for player movement.
 * @param offset_z Additional offset along the Z-axis, used for rendering or
 * bullet spawning.
 * @param model Pointer to the 3D model of the ship. Must not be NULL.
 * @param bullets Pointer to the shared bullet list for managing projectiles.
 * @return Pointer to a newly created Player instance, or NULL on failure.
 */
Player *newPlayer(float max_x, float max_y, float max_z, float offset_z,
                  ShipModel *model, BulletList *bullets) {
  if (!model) {
    return NULL;
  }
  Player *player = malloc(sizeof(Player));
  if (!player) {
    return NULL;
  }
  player->type = UNIT_TYPE_SOLDER;
  player->state = newUnitState();
  player->render = newPlayerRender(max_x, max_y, max_z, offset_z);
  player->model = model;
  player->bullets = bullets;
  return player;
}

/**
 * @brief Checks whether the movement direction has changed since the last
 * frame.
 *
 * Compares the current key input with the last recorded directional key state.
 * Used to reset acceleration when input direction changes.
 *
 * @param player Pointer to the player instance to check.
 * @return true if the direction key has changed, false otherwise.
 */
bool directionChanged(Player *player) {
  return (IsKeyDown(KEY_LEFT) &&
          player->render.movement.direction_x_key != KEY_LEFT) ||
         (IsKeyDown(KEY_RIGHT) &&
          player->render.movement.direction_x_key != KEY_RIGHT) ||
         (IsKeyDown(KEY_UP) &&
          player->render.movement.direction_z_key != KEY_UP) ||
         (IsKeyDown(KEY_DOWN) &&
          player->render.movement.direction_z_key != KEY_DOWN);
}

/**
 * @brief Updates player state, including position, acceleration, rotation, and
 * firing.
 *
 * This function:
 * - Handles keyboard input (arrows and space).
 * - Applies acceleration based on key press duration.
 * - Updates the player's position and rotation based on input.
 * - Spawns a bullet if spacebar is pressed and the cooldown has elapsed.
 * - Restores rotation angles to neutral when no directional keys are pressed.
 *
 * @param player Pointer to the player instance to update.
 */
void updatePlayer(Player *player) {
  if (!player) {
    return;
  }

  PlayerMovement *movement = &player->render.movement;
  PlayerVisualState *state = &player->render.state;
  PlayerPosition *position = &player->render.position;
  BulletList *bullets = player->bullets;

  double current_time = GetTime();
  double elapsed_last_bullet_spawn = current_time - bullets->last_spawn;

  if (IsKeyDown(KEY_SPACE) && elapsed_last_bullet_spawn > 0.2f) {
    Bullet bullet =
        newBullet(BULLET_MOVEMENT_DIRECTION_UP,
                  newBulletPosition(position->x, position->y,
                                    position->z + position->offset_z),
                  newBulletSize(1.0f, 1.0f, 1.0f), newBulletParameters(10, 10));
    insertBulletIntoList(player->bullets, bullet);
    bullets->last_spawn = current_time;
  }
  if (!(IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_UP) ||
        IsKeyDown(KEY_DOWN))) {
    if (state->rotate_x != 0) {
      if (state->rotate_x < 0) {
        state->rotate_x += state->rotate_step_x;
      } else {
        state->rotate_x -= state->rotate_step_x;
      }
    }
    if (state->rotate_z != 0) {
      if (state->rotate_z < 0) {
        state->rotate_z += state->rotate_step_z;
      } else {
        state->rotate_z -= state->rotate_step_z;
      }
    }
    return;
  }
  double elapsed = current_time - movement->last_key_press;
  movement->last_key_press = current_time;
  if (elapsed > ACCELERATION_DEALY || directionChanged(player)) {
    movement->acceleration = ACCELERATION_INIT;
  } else {
    movement->acceleration += ACCELERATION_STEP;
  }

  if (movement->acceleration > ACCELERATION_MAX) {
    movement->acceleration = ACCELERATION_MAX;
  }

  if (IsKeyDown(KEY_LEFT)) {
    position->x -= movement->acceleration;
    movement->direction_x_key = KEY_LEFT;
    state->rotate_z -= state->rotate_step_z;
    state->rotate_z = fabsf(state->rotate_z) >= state->max_rotate_z
                          ? -state->max_rotate_z
                          : state->rotate_z;
  }
  if (IsKeyDown(KEY_RIGHT)) {
    position->x += movement->acceleration;
    movement->direction_x_key = KEY_RIGHT;
    state->rotate_z += state->rotate_step_z;
    state->rotate_z = fabsf(state->rotate_z) >= state->max_rotate_z
                          ? state->max_rotate_z
                          : state->rotate_z;
  }
  if (IsKeyDown(KEY_UP)) {
    position->z -= movement->acceleration;
    movement->direction_z_key = KEY_UP;
    state->rotate_x += state->rotate_step_x;
    state->rotate_x = fabsf(state->rotate_x) >= state->max_rotate_x
                          ? state->max_rotate_x
                          : state->rotate_x;
  }
  if (IsKeyDown(KEY_DOWN)) {
    position->z += movement->acceleration;
    movement->direction_z_key = KEY_DOWN;
    state->rotate_x -= state->rotate_step_x;
    state->rotate_x = fabsf(state->rotate_x) >= state->max_rotate_x
                          ? -state->max_rotate_x
                          : state->rotate_x;
  }
}

/**
 * @brief Renders the player model and its debug box (if enabled).
 *
 * Applies transformations including translation and rotation before rendering
 * the 3D model. In debug mode, an additional bounding box is rendered using the
 * player's box model.
 *
 * @param player Pointer to the player instance to draw.
 */
void drawPlayer(Player *player) {
  if (!player)
    return;
  updatePlayer(player);

  Vector3 pos = {player->render.position.x, player->render.position.y,
                 player->render.position.z + player->render.position.offset_z};

  Matrix transform = MatrixTranslate(pos.x, pos.y, pos.z);

  Matrix rotX = MatrixRotateX(DEG2RAD * player->render.state.rotate_x);

  Matrix rotZ = MatrixRotateZ(DEG2RAD * player->render.state.rotate_z);

  Matrix rotY = MatrixRotateY(DEG2RAD * 180.0f);

  Matrix result = MatrixMultiply(rotX, rotZ);
  result = MatrixMultiply(result, rotY);
  result = MatrixMultiply(result, transform);
  Model model = player->model->model;
  model.transform = result;
  DrawModel(model, (Vector3){0, 0, 0}, 1.0f, WHITE);
  if (is_debug_mode) {
    if (is_debug_mode && player->model->box_model) {
      Model box_model = *player->model->box_model;
      box_model.transform = result;
      DrawModel(box_model, (Vector3){0, 0, 0}, 1.0f, RED);
    }
  }
}

/**
 * @brief Releases memory allocated for the player instance.
 *
 * Does not free associated resources like the model or bullet list.
 *
 * @param player Pointer to the player instance to destroy.
 */
void destroyPlayer(Player *player) {
  if (!player) {
    return;
  }
  free(player);
}
