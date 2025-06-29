#include "unit.h"
#include "../movement/movement.h"
#include <SDL.h>
#include <stdint.h>
#include <stdlib.h>

const uint16_t DEFAULT_UNIT_WIDTH = 64;
const uint16_t DEFAULT_UNIT_HEIGHT = 64;

const uint16_t UNIT_SPACE_VERTICAL = 42;
const uint16_t UNIT_SPACE_HORIZONTAL = 42;

const uint8_t DEFAULT_UNIT_HEALTH = 100;
const uint8_t DEFAULT_UNIT_ENERGY = 100;

UnitSize UnitSize_new() {
  UnitSize size;
  size.width = DEFAULT_UNIT_WIDTH;
  size.height = DEFAULT_UNIT_HEIGHT;
  return size;
};

UnitPosition UnitPosition_new() {
  UnitPosition position;
  position.x = 0;
  position.y = 0;
  position.ln = 0;
  position.col = 0;
  return position;
};

UnitRender UnitRender_new(UnitPosition position) {
  UnitRender render;
  render.position = position;
  render.size = UnitSize_new();
  render.action = MovementAction_new();
  render.last_frame = 0;
  return render;
}

UnitState UnitState_new() {
  UnitState state;
  state.health = DEFAULT_UNIT_HEALTH;
  state.energy = DEFAULT_UNIT_ENERGY;
  return state;
}

Unit Unit_new(UnitType ty) {
  Unit unit;
  unit.type = ty;
  unit.state = UnitState_new();
  unit.render = UnitRender_new(UnitPosition_new());
  return unit;
}

UnitNode *UnitNode_new(UnitNode *prev, Unit unit, int window_width) {
  UnitNode *node = malloc(sizeof(UnitNode));
  if (!node) {
    return NULL;
  }
  if (prev) {
    uint16_t prev_x = prev->self.render.position.x;
    uint16_t prev_y = prev->self.render.position.y;
    if (prev_x + DEFAULT_UNIT_WIDTH + UNIT_SPACE_HORIZONTAL < window_width) {
      unit.render.position.x =
          prev_x + prev->self.render.size.width + UNIT_SPACE_HORIZONTAL;
      unit.render.position.y = prev_y;
      unit.render.position.col = prev->self.render.position.col + 1;
      unit.render.position.ln = prev->self.render.position.ln;
    } else {
      unit.render.position.x = UNIT_SPACE_HORIZONTAL;
      unit.render.position.y =
          prev_y + prev->self.render.size.height + UNIT_SPACE_VERTICAL;
      unit.render.position.col = 0;
      unit.render.position.ln = prev->self.render.position.ln + 1;
    }
  } else {
    unit.render.position.x = UNIT_SPACE_HORIZONTAL;
    unit.render.position.y = UNIT_SPACE_VERTICAL;
    unit.render.position.col = 0;
    unit.render.position.ln = 0;
  }
  node->next = NULL;
  node->prev = prev;
  node->self = unit;
  return node;
}

void UnitNode_destroy(UnitNode *node) {
  if (node != NULL) {
    MovementAction_destroy(node->self.render.action);
    free(node);
  }
}

void Unit_render(SDL_Renderer *renderer, Unit *unit) {
  uint32_t current_time = SDL_GetTicks();
  MovementAction *action = unit->render.action;
  if (unit->render.last_frame < current_time &&
      current_time - unit->render.last_frame > 50) {
    MovementAction_iterate(action);
    unit->render.last_frame = current_time;
  }
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_Rect square = {unit->render.position.x, unit->render.position.y,
                     unit->render.size.width, unit->render.size.height};
  if (action) {
    square.x += action->x;
    square.y += action->y;
  }
  SDL_RenderFillRect(renderer, &square);
}

UnitList *UnitList_new(int count, int width) {
  UnitList *units = malloc(sizeof(UnitList));
  if (!units) {
    return NULL;
  }
  units->length = 0;
  units->head = NULL;
  units->tail = NULL;
  for (int i = count; i >= 0; i -= 1) {
    UnitList_insert(units, Unit_new(UNIT_TYPE_ENEMY), width);
    printf("Added unit %i\n", i);
  }
  return units;
}

void UnitList_destroy(UnitList *list) {
  UnitNode *node = list->head;
  while (node) {
    UnitNode *next = node->next;
    UnitNode_destroy(node);
    node = next;
  }
  list->head = list->tail = NULL;
  list->length = 0;
}

void UnitList_insert(UnitList *list, Unit unit, int window_width) {
  UnitNode *node = UnitNode_new(list->tail, unit, window_width);
  if (!node) {
    return;
  }
  if (list->length == 0) {
    list->head = list->tail = node;
  } else {
    list->tail->next = node;
    node->prev = list->tail;
    list->tail = node;
  }
  list->length += 1;
}

void UnitList_render(SDL_Renderer *renderer, UnitList *list) {
  UnitNode *node = list->head;
  while (node) {
    Unit_render(renderer, &node->self);
    node = node->next;
  }
}