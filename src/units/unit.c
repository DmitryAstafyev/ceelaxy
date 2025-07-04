#include "unit.h"
#include "../models/models.h"
#include "../movement/movement.h"
#include "raylib.h" // For Model, Texture2D, Shader
#include <raymath.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

const float DEFAULT_UNIT_WIDTH = 6.0;
const float DEFAULT_UNIT_HEIGHT = 6.0;

const float UNIT_SPACE_VERTICAL = 3.0;
const float UNIT_SPACE_HORIZONTAL = 3.0;

const uint8_t DEFAULT_UNIT_HEALTH = 100;
const uint8_t DEFAULT_UNIT_ENERGY = 100;

UnitSize newUnitSize() {
  UnitSize size;
  size.width = DEFAULT_UNIT_WIDTH;
  size.height = DEFAULT_UNIT_HEIGHT;
  return size;
};

UnitPosition newUnitPosition() {
  UnitPosition position;
  position.x = 0.0f;
  position.y = 0.0f;
  position.z = 0.0f;
  position.ln = 0;
  position.col = 0;
  return position;
};

UnitRender newUnitRender(UnitPosition position) {
  UnitRender render;
  render.position = position;
  render.size = newUnitSize();
  render.action = newMovementAction();
  render.last_frame = 0;
  return render;
}

UnitState newUnitState() {
  UnitState state;
  state.health = DEFAULT_UNIT_HEALTH;
  state.energy = DEFAULT_UNIT_ENERGY;
  return state;
}

Unit newUnit(UnitType ty, ShipModel *model) {
  Unit unit;
  unit.type = ty;
  unit.state = newUnitState();
  unit.render = newUnitRender(newUnitPosition());
  unit.model = model;
  return unit;
}

UnitNode *newUnitNode(UnitNode *prev, Unit unit, int max_col, int max_ln,
                      float mid_x, float mid_y) {
  UnitNode *node = malloc(sizeof(UnitNode));
  if (!node) {
    return NULL;
  }
  int prev_col, prev_ln;
  if (prev) {
    unit.render.position.col = prev->self.render.position.col + 1;
    prev_ln = prev->self.render.position.ln;
  } else {
    unit.render.position.col = 0;
    prev_ln = 0;
  }
  if (unit.render.position.col == max_col) {
    unit.render.position.ln = prev_ln + 1;
    unit.render.position.col = 0;
  } else {
    unit.render.position.ln = prev_ln;
  }
  unit.render.position.x =
      (DEFAULT_UNIT_WIDTH + UNIT_SPACE_HORIZONTAL) * unit.render.position.col -
      mid_x;
  unit.render.position.y =
      (DEFAULT_UNIT_WIDTH + UNIT_SPACE_HORIZONTAL) * unit.render.position.ln -
      mid_y;
  unit.render.position.z = 0.0f;
  node->next = NULL;
  node->prev = prev;
  node->self = unit;
  return node;
}

void destroyUnitNode(UnitNode *node) {
  if (node != NULL) {
    destroyMovementAction(node->self.render.action);
    free(node);
  }
}

void drawUnit(Unit *unit) {
  if (!unit) {
    return;
  }
  // double current_time = GetTime();
  // MovementAction *action = unit->render.action;
  // if (unit->render.last_frame < current_time &&
  //     current_time - unit->render.last_frame > 50) {
  //   iterateMovementAction(action);
  //   unit->render.last_frame = current_time;
  // }
  // iterateMovementAction(action);

  // DrawModelEx(unit->model->model,
  //             (Vector3){unit->render.position.x + action->x,
  //                       unit->render.position.y + action->y,
  //                       unit->render.position.z},
  //             (Vector3){0, 1, 0}, 0.0f, (Vector3){1, 1, 1}, WHITE);
  DrawModelEx(unit->model->model,
              (Vector3){unit->render.position.x, unit->render.position.y,
                        unit->render.position.z},
              (Vector3){0, 0, 0}, 0.0f, (Vector3){1, 1, 1}, WHITE);
}

UnitList *newUnitList(int count, ShipModel *model, int max_col, int max_ln) {
  UnitList *units = malloc(sizeof(UnitList));
  if (!units) {
    return NULL;
  }
  units->length = 0;
  units->head = NULL;
  units->tail = NULL;
  float mid_x = ((DEFAULT_UNIT_WIDTH + UNIT_SPACE_HORIZONTAL) * max_col -
                 UNIT_SPACE_HORIZONTAL) /
                2;
  float mid_y = ((DEFAULT_UNIT_HEIGHT + UNIT_SPACE_VERTICAL) * max_ln -
                 UNIT_SPACE_VERTICAL) /
                2;
  for (int i = count - 1; i >= 0; i -= 1) {
    insertToUnitList(units, newUnit(UNIT_TYPE_ENEMY, model), max_col, max_ln,
                     mid_x, mid_x);
    printf("Added unit %i\n", i);
  }
  return units;
}

void destroyUnitList(UnitList *list) {
  UnitNode *node = list->head;
  while (node) {
    UnitNode *next = node->next;
    destroyUnitNode(node);
    node = next;
  }
  list->head = list->tail = NULL;
  list->length = 0;
}

void insertToUnitList(UnitList *list, Unit unit, int max_col, int max_ln,
                      float mid_x, float mid_y) {
  UnitNode *node = newUnitNode(list->tail, unit, max_col, max_ln, mid_x, mid_y);
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

void drawUnits(UnitList *list) {
  UnitNode *node = list->head;
  for (int i = 0; i < list->length; i += 1) {
    if (!node) {
      break;
    }
    drawUnit(&node->self);
    node = node->next;
  }
}
