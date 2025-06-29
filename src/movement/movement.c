#include "./movement.h"
#include <stdint.h>
#include <stdlib.h>

const uint16_t MOVEMENT_STEP_X = 1;
const uint16_t MOVEMENT_STEP_Y = 1;
const uint16_t MOVEMENT_MAX_X = 20;
const uint16_t MOVEMENT_MAX_Y = 20;

#define MOVEMENT_HORIZONTAL_MASK                                               \
  (MOVEMENT_DIRECTION_LEFT | MOVEMENT_DIRECTION_RIGHT)

#define MOVEMENT_VERTICAL_MASK (MOVEMENT_DIRECTION_UP | MOVEMENT_DIRECTION_DOWN)

MovementAction *MovementAction_new() {
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
  MovementAction_rand_speed(action);
  return action;
}

void MovementAction_rand_speed(MovementAction *action) {
  if (!action) {
    return;
  }
  action->step_x = rand() % 2 ? MOVEMENT_STEP_X : MOVEMENT_STEP_X + 1;
  action->step_y = rand() % 2 ? MOVEMENT_STEP_Y : MOVEMENT_STEP_Y + 1;
}

void MovementAction_iterate(MovementAction *action) {
  if (!action) {
    return;
  }
  if (action->direction & MOVEMENT_HORIZONTAL_MASK) {
    if (abs(action->x) <= action->max_x) {
      action->x += (action->step_x) *
                   (action->direction & MOVEMENT_DIRECTION_LEFT ? -1 : 1);
    } else {
      action->x = (action->max_x - 1) * (action->x > 0 ? 1 : -1);
      if (action->direction & MOVEMENT_DIRECTION_LEFT) {
        action->direction &= ~MOVEMENT_DIRECTION_LEFT;
        action->direction |= MOVEMENT_DIRECTION_RIGHT;
      } else {
        action->direction &= ~MOVEMENT_DIRECTION_RIGHT;
        action->direction |= MOVEMENT_DIRECTION_LEFT;
      }
      MovementAction_rand_speed(action);
    }
  }
  if (action->direction & MOVEMENT_VERTICAL_MASK) {
    if (abs(action->y) <= action->max_y) {
      action->y += (action->step_y) *
                   (action->direction & MOVEMENT_DIRECTION_UP ? -1 : 1);
    } else {
      action->y = (action->max_y - 1) * (action->y > 0 ? 1 : -1);
      if (action->direction & MOVEMENT_DIRECTION_UP) {
        action->direction &= ~MOVEMENT_DIRECTION_UP;
        action->direction |= MOVEMENT_DIRECTION_DOWN;
      } else {
        action->direction &= ~MOVEMENT_DIRECTION_DOWN;
        action->direction |= MOVEMENT_DIRECTION_UP;
      }
      MovementAction_rand_speed(action);
    }
  }
}

void MovementAction_destroy(MovementAction *action) {
  if (action) {
    free(action);
  }
}