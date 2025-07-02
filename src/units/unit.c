#include "unit.h"
#include "../models/models.h"
#include "../movement/movement.h"
#include "raylib.h" // For Model, Texture2D, Shader
#include <raymath.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

const uint16_t DEFAULT_UNIT_WIDTH = 64;
const uint16_t DEFAULT_UNIT_HEIGHT = 64;

const uint16_t UNIT_SPACE_VERTICAL = 42;
const uint16_t UNIT_SPACE_HORIZONTAL = 42;

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
  position.x = 0;
  position.y = 0;
  position.z = 10;
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

UnitNode *newUnitNode(UnitNode *prev, Unit unit, int window_width) {
  UnitNode *node = malloc(sizeof(UnitNode));
  if (!node) {
    return NULL;
  }
  if (prev) {
    float prev_x = prev->self.render.position.x;
    float prev_y = prev->self.render.position.y;
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

void destroyUnitNode(UnitNode *node) {
  if (node != NULL) {
    destroyMovementAction(node->self.render.action);
    free(node);
  }
}

Matrix getMatrixUnit(Unit *unit) {
  // double current_time = GetTime();
  // MovementAction *action = unit->render.action;
  // if (unit->render.last_frame < current_time &&
  //     current_time - unit->render.last_frame > 50) {
  //   iterateMovementAction(action);
  //   unit->render.last_frame = current_time;
  // }
  printf("%f,%f,%f\n", unit->render.position.x, unit->render.position.y,
         unit->render.position.z);
  return MatrixTranslate(unit->render.position.x, unit->render.position.y,
                         unit->render.position.z);
  // return MatrixTranslate(unit->render.position.x + action->x,
  //                        unit->render.position.y + action->y,
  //                        unit->render.position.z);
}

UnitList *newUnitList(int count, int width, ShipModel *model) {
  UnitList *units = malloc(sizeof(UnitList));
  if (!units) {
    return NULL;
  }
  units->length = 0;
  units->head = NULL;
  units->tail = NULL;
  for (int i = count - 1; i >= 0; i -= 1) {
    insertToUnitList(units, newUnit(UNIT_TYPE_ENEMY, model), width);
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

void insertToUnitList(UnitList *list, Unit unit, int window_width) {
  UnitNode *node = newUnitNode(list->tail, unit, window_width);
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

Matrix *getMatrixFromUnitList(UnitList *list) {
  UnitNode *node = list->head;
  Matrix *matrices = malloc(sizeof(Matrix) * list->length);
  int added = 0;
  for (int i = 0; i < list->length; i += 1) {
    if (!node) {
      break;
    }
    matrices[i] = getMatrixUnit(&node->self);
    node = node->next;
    added += 1;
  }
  if (added != list->length) {
    fprintf(stderr,
            "[Units] Fail to get list of Matrix for all units; expected %i "
            "units, but added %i\n",
            list->length, added);
    exit(EXIT_FAILURE);
    free(matrices);
  }
  return matrices;
}