/**
 * @file movement.c
 * @brief Implements dynamic movement behavior for game entities, including
 * oscillation, direction reversal, per-axis motion, and randomization of
 * movement speed.
 */

#include "./movement.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

/// Default movement step along the X axis per frame.
const float MOVEMENT_STEP_X = 0.01f;

/// Default movement step along the Y axis per frame.
const float MOVEMENT_STEP_Y = 0.01f;

/// Default movement step along the Z axis per frame.
const float MOVEMENT_STEP_Z = 0.01f;

/// Maximum allowed oscillation along the X axis.
const float MOVEMENT_MAX_X = 1.0f;

/// Maximum allowed oscillation along the Y axis.
const float MOVEMENT_MAX_Y = 1.0f;

/// Maximum allowed oscillation along the Z axis.
const float MOVEMENT_MAX_Z = 1.0f;

/// Bitmask for checking horizontal movement (left or right).
#define MOVEMENT_X_MASK (MOVEMENT_DIRECTION_LEFT | MOVEMENT_DIRECTION_RIGHT)

/// Bitmask for checking vertical movement (up or down).
#define MOVEMENT_Y_MASK (MOVEMENT_DIRECTION_UP | MOVEMENT_DIRECTION_DOWN)

/// Bitmask for checking forward/backward movement (Z axis).
#define MOVEMENT_Z_MASK                                                        \
  (MOVEMENT_DIRECTION_FORWARD | MOVEMENT_DIRECTION_BACKWARD)

/**
 * @brief Allocates and initializes a new MovementAction with default
 * oscillation behavior.
 *
 * Sets a random initial direction, bounds, and angle/rotation constraints.
 * Also assigns randomized speed values via randSpeedMovementAction().
 *
 * @return Pointer to a newly allocated MovementAction, or NULL on failure.
 */
MovementAction *newMovementAction() {
  MovementAction *action = malloc(sizeof(MovementAction));
  if (!action) {
    return NULL;
  }
  action->direction =
      (rand() % 2 ? MOVEMENT_DIRECTION_LEFT : MOVEMENT_DIRECTION_RIGHT) |
      (rand() % 2 ? MOVEMENT_DIRECTION_FORWARD : MOVEMENT_DIRECTION_BACKWARD);
  action->max_x = MOVEMENT_MAX_X;
  action->max_y = MOVEMENT_MAX_Y;
  action->max_z = MOVEMENT_MAX_Z;
  action->x = 0.0f;
  action->y = 0.0f;
  action->z = 0.0f;
  action->max_rotate_x = 10.0f;
  action->max_rotate_z = 15.0f;
  action->max_rotate_y = 0;
  action->rotate_x = 0.0f;
  action->rotate_y = 0.0f;
  action->rotate_z = 0.0f;
  action->max_angle = 15.0f;
  action->angle = 0.0f;
  randSpeedMovementAction(action);
  return action;
}

/**
 * @brief Generates a random float value within a specified range.
 *
 * @param min Minimum possible value.
 * @param max Maximum possible value.
 * @return Random float in the range [min, max].
 */
float randomFloatInRange(float min, float max) {
  return min + ((float)rand() / (float)RAND_MAX) * (max - min);
}

/**
 * @brief Assigns random step values for X, Y, and Z movement axes.
 *
 * Intended to be used during direction reversals or initialization to add
 * motion variability.
 *
 * @param action Pointer to the MovementAction to modify.
 */
void randSpeedMovementAction(MovementAction *action) {
  if (!action) {
    return;
  }
  action->step_x = randomFloatInRange(0.01f, 0.05f);
  action->step_y = randomFloatInRange(0.01f, 0.05f);
  action->step_z = randomFloatInRange(0.01f, 0.05f);
}

/**
 * @brief Advances the movement state of the entity for the current frame.
 *
 * This function:
 * - Applies step-based motion along X, Y, and Z axes according to direction.
 * - Reverses direction when maximum displacement is reached.
 * - Updates rotation (rotate_x, rotate_z) and visual angle for animation.
 * - Introduces natural oscillation-like behavior.
 *
 * @param action Pointer to the MovementAction to iterate.
 */
void iterateMovementAction(MovementAction *action) {
  if (!action) {
    return;
  }
  if (action->direction & MOVEMENT_X_MASK) {
    if (fabsf(action->x) <= action->max_x) {
      action->x += (action->step_x) *
                   (action->direction & MOVEMENT_DIRECTION_LEFT ? -1 : 1);
    } else {
      action->x = action->max_x * (action->x > 0 ? 1 : -1);
      if (action->direction & MOVEMENT_DIRECTION_LEFT) {
        action->direction &= ~MOVEMENT_DIRECTION_LEFT;
        action->direction |= MOVEMENT_DIRECTION_RIGHT;
      } else {
        action->direction &= ~MOVEMENT_DIRECTION_RIGHT;
        action->direction |= MOVEMENT_DIRECTION_LEFT;
      }
      randSpeedMovementAction(action);
    }
    action->rotate_z = -action->max_rotate_z * (action->x / action->max_x);
    action->angle = action->max_angle *
                    ((action->max_x - fabsf(action->x)) - action->max_x);
  } else {
    action->rotate_z = 0.0f;
  }
  if (action->direction & MOVEMENT_Y_MASK) {
    if (fabsf(action->y) <= action->max_y) {
      action->y += (action->step_y) *
                   (action->direction & MOVEMENT_DIRECTION_UP ? -1 : 1);
    } else {
      action->y = action->max_y * (action->y > 0 ? 1 : -1);
      if (action->direction & MOVEMENT_DIRECTION_UP) {
        action->direction &= ~MOVEMENT_DIRECTION_UP;
        action->direction |= MOVEMENT_DIRECTION_DOWN;
      } else {
        action->direction &= ~MOVEMENT_DIRECTION_DOWN;
        action->direction |= MOVEMENT_DIRECTION_UP;
      }
      randSpeedMovementAction(action);
    }
  }
  if (action->direction & MOVEMENT_Z_MASK) {
    if (fabsf(action->z) <= action->max_z) {
      action->z += (action->step_z) *
                   (action->direction & MOVEMENT_DIRECTION_FORWARD ? -1 : 1);
    } else {
      action->z = action->max_z * (action->z > 0 ? 1 : -1);
      if (action->direction & MOVEMENT_DIRECTION_FORWARD) {
        action->direction &= ~MOVEMENT_DIRECTION_FORWARD;
        action->direction |= MOVEMENT_DIRECTION_BACKWARD;
      } else {
        action->direction &= ~MOVEMENT_DIRECTION_BACKWARD;
        action->direction |= MOVEMENT_DIRECTION_FORWARD;
      }
      randSpeedMovementAction(action);
    }
    action->rotate_x = action->max_rotate_x * (action->z / action->max_z);
    action->angle = action->max_angle *
                    ((action->max_z - fabsf(action->z)) - action->max_z);
  } else {
    action->rotate_x = 0.0f;
  }
}

/**
 * @brief Deallocates memory associated with the given MovementAction.
 *
 * @param action Pointer to the MovementAction to destroy.
 */
void destroyMovementAction(MovementAction *action) {
  if (action) {
    free(action);
  }
}