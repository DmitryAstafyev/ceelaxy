#include "unit.h"
#include "../bullets/bullets.h"
#include "../models/models.h"
#include "../movement/movement.h"
#include "../utils/debug.h"
#include "raylib.h" // For Model, Texture2D, Shader
#include <math.h>
#include <raymath.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

const float DEFAULT_UNIT_WIDTH = 6.0;
const float DEFAULT_UNIT_HEIGHT = 6.0;

const float UNIT_SPACE_VERTICAL = 6.0;
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
  state.hit_time = 0.0f;
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
                      float mid_x, float z_offset) {
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
  unit.render.position.z =
      (DEFAULT_UNIT_HEIGHT + UNIT_SPACE_VERTICAL) * unit.render.position.ln -
      z_offset;
  unit.render.position.y = 0.0f;
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
  MovementAction *action = unit->render.action;
  iterateMovementAction(action);

  UnitPosition *position = &unit->render.position;
  double current = GetTime();
  bool hit = current > BULLET_HIT_SEN_TIME &&
             current - unit->state.hit_time < BULLET_HIT_SEN_TIME;
  if (hit) {
    removeShipModelTexture(unit->model, RED);
  }
  DrawModelEx(unit->model->model,
              (Vector3){position->x + action->x, position->y + action->y,
                        position->z + action->z},
              (Vector3){action->rotate_x, action->rotate_y, action->rotate_z},
              action->angle, (Vector3){1, 1, 1}, hit ? RED : WHITE);
  if (hit) {
    restoreShipModelTexture(unit->model);
  }
  if (is_debug_mode && unit->model->box_model) {
    DrawModelEx(*unit->model->box_model,
                (Vector3){position->x + action->x, position->y + action->y,
                          position->z + action->z},
                (Vector3){action->rotate_x, action->rotate_y, action->rotate_z},
                action->angle, (Vector3){1, 1, 1}, RED);
  }
}

BoundingBox getUnitBoundingBox(Unit *unit) {
  ShipBoundingBox *box = &unit->model->box;
  UnitRender *render = &unit->render;
  MovementAction *action = render->action;

  float x = render->position.x + (action ? action->x : 0);
  float y = render->position.y + (action ? action->y : 0);
  float z = render->position.z + (action ? action->z : 0);

  Vector3 position = {x, y, z};

  BoundingBox local = {.min = {-box->by_x / 2, -box->by_y / 2, -box->by_z / 2},
                       .max = {box->by_x / 2, box->by_y / 2, box->by_z / 2}};

  Matrix transform = MatrixTranslate(position.x, position.y, position.z);
  if (action) {
    Matrix rotX = MatrixRotateX(DEG2RAD * action->rotate_x);
    Matrix rotZ = MatrixRotateZ(DEG2RAD * action->rotate_z);
    Matrix rotY = MatrixRotateY(DEG2RAD * action->rotate_y);
    Matrix rotAll = MatrixMultiply(MatrixMultiply(rotX, rotZ), rotY);
    transform = MatrixMultiply(rotAll, transform);
  }

  Vector3 corners[8] = {
      {local.min.x, local.min.y, local.min.z},
      {local.min.x, local.min.y, local.max.z},
      {local.min.x, local.max.y, local.min.z},
      {local.min.x, local.max.y, local.max.z},
      {local.max.x, local.min.y, local.min.z},
      {local.max.x, local.min.y, local.max.z},
      {local.max.x, local.max.y, local.min.z},
      {local.max.x, local.max.y, local.max.z},
  };

  BoundingBox world = {.min = Vector3Transform(corners[0], transform),
                       .max = Vector3Transform(corners[0], transform)};

  for (int i = 1; i < 8; i++) {
    Vector3 transformed = Vector3Transform(corners[i], transform);
    world.min = Vector3Min(world.min, transformed);
    world.max = Vector3Max(world.max, transformed);
  }

  return world;
}

UnitList *newUnitList(int count, ShipModel *model, int max_col, int max_ln,
                      float z_offset) {
  UnitList *units = malloc(sizeof(UnitList));
  if (!units) {
    return NULL;
  }
  units->length = 0;
  units->head = NULL;
  units->tail = NULL;
  float unit_full_width = DEFAULT_UNIT_WIDTH + UNIT_SPACE_HORIZONTAL;
  float unit_full_height = DEFAULT_UNIT_HEIGHT + UNIT_SPACE_VERTICAL;

  float mid_x = (unit_full_width * max_col) / 2.0f - unit_full_width / 2.0f;
  for (int i = count - 1; i >= 0; i -= 1) {
    insertToUnitList(units, newUnit(UNIT_TYPE_ENEMY, model), max_col, max_ln,
                     mid_x, z_offset);
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
                      float mid_x, float z_offset) {
  UnitNode *node =
      newUnitNode(list->tail, unit, max_col, max_ln, mid_x, z_offset);
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
