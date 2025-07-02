#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <stdint.h>

typedef enum MovementDirection {
  MOVEMENT_DIRECTION_NONE = 0x00,
  MOVEMENT_DIRECTION_LEFT = 0x01,  // 0001
  MOVEMENT_DIRECTION_RIGHT = 0x02, // 0010
  MOVEMENT_DIRECTION_UP = 0x04,    // 0100
  MOVEMENT_DIRECTION_DOWN = 0x08   // 1000
} MovementDirection;

typedef struct MovementAction {
  uint8_t direction;
  uint16_t step_x;
  uint16_t step_y;
  uint16_t max_x;
  uint16_t max_y;
  int16_t x;
  int16_t y;
} MovementAction;

MovementAction *newMovementAction();

void iterateMovementAction(MovementAction *action);

void destroyMovementAction(MovementAction *action);

void randSpeedMovementAction(MovementAction *action);

#endif