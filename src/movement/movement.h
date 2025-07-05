#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <stdint.h>

typedef enum MovementDirection {
  MOVEMENT_DIRECTION_NONE = 0x00,
  MOVEMENT_DIRECTION_LEFT = 0x01,    // 0001
  MOVEMENT_DIRECTION_RIGHT = 0x02,   // 0010
  MOVEMENT_DIRECTION_UP = 0x04,      // 0100
  MOVEMENT_DIRECTION_DOWN = 0x08,    // 1000
  MOVEMENT_DIRECTION_FORWARD = 0x10, // 010000
  MOVEMENT_DIRECTION_BACKWARD = 0x20 // 100000
} MovementDirection;

typedef struct MovementAction {
  uint8_t direction;
  float step_x;
  float step_y;
  float step_z;
  float rotate_x;
  float rotate_y;
  float rotate_z;
  float max_rotate_x;
  float max_rotate_y;
  float max_rotate_z;
  float max_angle;
  float angle;
  float max_x;
  float max_y;
  float max_z;
  float x;
  float y;
  float z;
} MovementAction;

MovementAction *newMovementAction();

void iterateMovementAction(MovementAction *action);

void destroyMovementAction(MovementAction *action);

void randSpeedMovementAction(MovementAction *action);

#endif