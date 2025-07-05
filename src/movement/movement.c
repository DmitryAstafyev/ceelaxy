#include "./movement.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

const float MOVEMENT_STEP_X = 0.01f;
const float MOVEMENT_STEP_Y = 0.01f;
const float MOVEMENT_MAX_X = 1.0f;
const float MOVEMENT_MAX_Y = 1.0f;

#define MOVEMENT_HORIZONTAL_MASK                                               \
  (MOVEMENT_DIRECTION_LEFT | MOVEMENT_DIRECTION_RIGHT)

#define MOVEMENT_VERTICAL_MASK (MOVEMENT_DIRECTION_UP | MOVEMENT_DIRECTION_DOWN)

MovementAction *newMovementAction() {
  MovementAction *action = malloc(sizeof(MovementAction));
  if (!action) {
    return NULL;
  }
  action->direction =
      (rand() % 2 ? MOVEMENT_DIRECTION_LEFT : MOVEMENT_DIRECTION_RIGHT) |
      (rand() % 2 ? MOVEMENT_DIRECTION_UP : MOVEMENT_DIRECTION_DOWN);
  action->max_x = MOVEMENT_MAX_X;
  action->max_y = MOVEMENT_MAX_Y;
  action->x = 0;
  action->y = 0;
  randSpeedMovementAction(action);
  return action;
}

float randomFloatInRange(float min, float max) {
  return min + ((float)rand() / (float)RAND_MAX) * (max - min);
}

void randSpeedMovementAction(MovementAction *action) {
  if (!action) {
    return;
  }
  action->step_x = randomFloatInRange(0.01f, 0.05f);
  action->step_y = randomFloatInRange(0.01f, 0.05f);
}

void iterateMovementAction(MovementAction *action) {
  if (!action) {
    return;
  }
  if (action->direction & MOVEMENT_HORIZONTAL_MASK) {
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
  }
  if (action->direction & MOVEMENT_VERTICAL_MASK) {
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
}

void destroyMovementAction(MovementAction *action) {
  if (action) {
    free(action);
  }
}