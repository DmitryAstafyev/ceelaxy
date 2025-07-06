/**
 * @file movement.h
 * @brief Declares data structures and functions for handling unit movement,
 * including direction, velocity, rotation, and animation state.
 */
#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <stdint.h>

/**
 * @brief Bitmask enum defining movement directions in 3D space.
 *
 * Can be combined using bitwise OR to represent compound directions.
 */
typedef enum MovementDirection {
  MOVEMENT_DIRECTION_NONE = 0x00,    ///< No movement.
  MOVEMENT_DIRECTION_LEFT = 0x01,    ///< Movement to the left (-X).
  MOVEMENT_DIRECTION_RIGHT = 0x02,   ///< Movement to the right (+X).
  MOVEMENT_DIRECTION_UP = 0x04,      ///< Movement upward (+Y).
  MOVEMENT_DIRECTION_DOWN = 0x08,    ///< Movement downward (-Y).
  MOVEMENT_DIRECTION_FORWARD = 0x10, ///< Movement forward (-Z).
  MOVEMENT_DIRECTION_BACKWARD = 0x20 ///< Movement backward (+Z).
} MovementDirection;

/**
 * @brief Represents the full dynamic movement state of a unit.
 *
 * Includes direction, movement speed per axis, rotation angles, bounds,
 * and accumulated displacement for animations or physics effects.
 */
typedef struct MovementAction {
  uint8_t direction;  ///< Movement direction as bitmask (MovementDirection).
  float step_x;       ///< Movement speed or step size along X axis.
  float step_y;       ///< Movement speed or step size along Y axis.
  float step_z;       ///< Movement speed or step size along Z axis.
  float rotate_x;     ///< Rotation around X axis (degrees).
  float rotate_y;     ///< Rotation around Y axis (degrees).
  float rotate_z;     ///< Rotation around Z axis (degrees).
  float max_rotate_x; ///< Max rotation allowed on X axis.
  float max_rotate_y; ///< Max rotation allowed on Y axis.
  float max_rotate_z; ///< Max rotation allowed on Z axis.
  float max_angle;    ///< Max general rotation angle.
  float angle;        ///< Current general rotation angle.
  float max_x;        ///< Max position delta along X.
  float max_y;        ///< Max position delta along Y.
  float max_z;        ///< Max position delta along Z.
  float x;            ///< Accumulated offset along X axis.
  float y;            ///< Accumulated offset along Y axis.
  float z;            ///< Accumulated offset along Z axis.
} MovementAction;

/**
 * @brief Allocates and initializes a new MovementAction with default values.
 *
 * @return Pointer to a newly allocated MovementAction, or NULL on failure.
 */
MovementAction *newMovementAction();

/**
 * @brief Advances the current movement and rotation values based on internal
 * state.
 *
 * Applies per-frame updates to rotation and positional offsets to simulate
 * motion.
 *
 * @param action Pointer to the MovementAction to update.
 */
void iterateMovementAction(MovementAction *action);

/**
 * @brief Frees memory used by a MovementAction.
 *
 * @param action Pointer to the MovementAction to destroy.
 */
void destroyMovementAction(MovementAction *action);

/**
 * @brief Assigns random movement speeds and directions to the action.
 *
 * Useful for initializing enemies or simulating unpredictable motion.
 *
 * @param action Pointer to the MovementAction to modify.
 */
void randSpeedMovementAction(MovementAction *action);

#endif