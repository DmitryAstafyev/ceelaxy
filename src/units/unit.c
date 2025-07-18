/**
 * @file unit.c
 * @brief Implements unit-related creation and initialization logic,
 * including size, position, state, and rendering configuration.
 */
#include "unit.h"
#include "../bullets/bullets.h"
#include "../models/models.h"
#include "../movement/movement.h"
#include "../units/explosion.h"
#include "../utils/debug.h"
#include "raylib.h"
#include <math.h>
#include <raymath.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/// Default width (in world units) assigned to all units.
const float DEFAULT_UNIT_WIDTH = 6.0;

/// Default height (in world units) assigned to all units.
const float DEFAULT_UNIT_HEIGHT = 6.0;

/// Vertical spacing between units when placed in formation.
const float UNIT_SPACE_VERTICAL = 6.0;

/// Horizontal spacing between units when placed in formation.
const float UNIT_SPACE_HORIZONTAL = 3.0;

/// Default health value for any newly created unit.
const uint8_t DEFAULT_UNIT_HEALTH = 100;

/// Default energy value for any newly created unit.
const uint8_t DEFAULT_UNIT_ENERGY = 100;

/**
 * @brief Initializes and returns a UnitSize structure with default dimensions.
 *
 * @return A UnitSize structure with predefined width and height.
 */
UnitSize newUnitSize() {
  UnitSize size;
  size.width = DEFAULT_UNIT_WIDTH;
  size.height = DEFAULT_UNIT_HEIGHT;
  return size;
};

/**
 * @brief Initializes and returns a UnitPosition structure with origin
 * coordinates and default grid mapping values.
 *
 * @return A UnitPosition structure with zeroed coordinates and grid indices.
 */
UnitPosition newUnitPosition() {
  UnitPosition position;
  position.x = 0.0f;
  position.y = 0.0f;
  position.z = 0.0f;
  position.z_max_area = 300.0f;
  position.ln = 0;
  position.col = 0;
  return position;
};

/**
 * @brief Creates a UnitRender structure with default visibility, size, and
 * movement action, based on a given position.
 *
 * @param position Initial world position of the unit.
 * @return A fully initialized UnitRender structure.
 */
UnitRender newUnitRender(UnitPosition position) {
  UnitRender render;
  render.position = position;
  render.size = newUnitSize();
  render.action = newMovementAction();
  render.last_frame = 0;
  render.visible = true;
  return render;
}

/**
 * @brief Initializes and returns a UnitState with full health and energy.
 *
 * @return A UnitState structure with default health, energy, and zeroed hit
 * time.
 */
UnitState newUnitState() {
  UnitState state;
  state.health = DEFAULT_UNIT_HEALTH;
  state.energy = DEFAULT_UNIT_ENERGY;
  state.hit_time = 0.0f;
  return state;
}

/**
 * @brief Constructs a new Unit object with specified type and model.
 *
 * Initializes the internal state, position, and render configuration using
 * default values.
 *
 * @param ty Type of the unit (e.g., player or enemy).
 * @param model Pointer to the ship model used for rendering this unit.
 * @return A fully constructed Unit structure.
 */
Unit newUnit(UnitType ty, ShipModel *model) {
  Unit unit;
  unit.type = ty;
  unit.state = newUnitState();
  unit.render = newUnitRender(newUnitPosition());
  unit.model = model;
  unit.explosion = NULL;
  return unit;
}

/**
 * @brief Creates a new UnitNode with proper grid-based positioning.
 *
 * Automatically assigns column and line (ln) indices based on the previous node
 * and positions the unit accordingly in a grid layout.
 *
 * @param prev Pointer to the previous node in the list, or NULL if first.
 * @param unit The Unit instance to store.
 * @param max_col Maximum number of columns in the grid.
 * @param max_ln Maximum number of rows (lines) in the grid.
 * @param mid_x Horizontal center for centering the grid.
 * @param z_offset Offset to apply to the Z position of the unit.
 * @return Pointer to the allocated UnitNode, or NULL on failure.
 */
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

/**
 * @brief Destroys a single UnitNode, including its movement action.
 *
 * @param node Pointer to the node to destroy.
 */
void destroyUnitNode(UnitNode *node) {
  if (node != NULL) {
    destroyMovementAction(node->self.render.action);
    if (node->self.explosion) {
      destroyExplosionState(node->self.explosion);
    }
    free(node);
  }
}

/**
 * @brief Updates the falling animation of a destroyed unit.
 *
 * If the unit's health is zero, its Y and Z positions are adjusted to simulate
 * falling. Rotation and oscillation are also applied. When it falls out of
 * range, it's marked invisible.
 *
 * @param unit Pointer to the unit to update.
 * @param deltaTime Time elapsed since last frame.
 */
void updateDestroyedUnitFall(Unit *unit, float deltaTime) {
  if (!unit || unit->state.health > 0)
    return;

  MovementAction *action = unit->render.action;
  UnitPosition *position = &unit->render.position;

  position->y -= 40.0f * deltaTime;

  position->z -= 50.0f * deltaTime;

  action->x = 2.5f * sinf(GetTime() * 5.0f);

  action->rotate_x = 0.0f;
  action->rotate_y = 10.0f;
  action->rotate_z = 0.0f;

  action->angle += 360.0f * deltaTime;

  if (action->angle > 360.0f) {
    action->angle -= 360.0f;
  }
  if (fabsf(position->z + action->z) > fabsf(position->z_max_area)) {
    unit->render.visible = false;
  }
}

/**
 * @brief Draws a single unit, applying animation, movement, hit indication, and
 * transformations.
 *
 * If the unit was recently hit, it's rendered red temporarily. If health is
 * zero, the falling animation is applied.
 *
 * In debug mode, the bounding box model is also rendered.
 *
 * @param unit Pointer to the unit to draw.
 */
void drawUnit(Unit *unit, Camera3D *camera, ExplosionModelList *explosions) {
  if (!unit) {
    return;
  }

  UnitPosition *position = &unit->render.position;
  double current = GetTime();
  bool hit = current > BULLET_HIT_SEN_TIME &&
             current - unit->state.hit_time < BULLET_HIT_SEN_TIME;
  if (hit) {
    setShipModelColor(unit->model, RED);
  }

  MovementAction *action = unit->render.action;

  if (unit->state.health == 0) {
    updateDestroyedUnitFall(unit, GetFrameTime());
    if (!unit->explosion) {
      unit->explosion = newExplosionState(&explosions->head->self);
    }
    if (unit->explosion) {
      drawExplosionState(unit->explosion, *camera,
                         (Vector3){position->x + action->x,
                                   position->y + action->y,
                                   position->z + action->z});
    }
  } else {
    iterateMovementAction(action);
  }
  DrawModelEx(unit->model->model,
              (Vector3){position->x + action->x, position->y + action->y,
                        position->z + action->z},
              (Vector3){action->rotate_x, action->rotate_y, action->rotate_z},
              action->angle, (Vector3){1, 1, 1}, hit ? RED : WHITE);
  if (hit) {
    setShipModelColor(unit->model, WHITE);
  }
  if (is_debug_mode && unit->model->box_model) {
    DrawModelEx(*unit->model->box_model,
                (Vector3){position->x + action->x, position->y + action->y,
                          position->z + action->z},
                (Vector3){action->rotate_x, action->rotate_y, action->rotate_z},
                action->angle, (Vector3){1, 1, 1}, RED);
  }
}

/**
 * @brief Calculates the world-space bounding box of the unit model, including
 * rotation.
 *
 * Applies the unit's movement offset and rotation to compute the transformed
 * box.
 *
 * @param unit Pointer to the unit.
 * @return A BoundingBox in world coordinates.
 */
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

/**
 * @brief Allocates and initializes a UnitList with a given number of enemy
 * units.
 *
 * Positions the units in a grid based on the column and row count.
 *
 * @param count Number of enemy units to spawn.
 * @param model Shared model used for all units.
 * @param max_col Maximum columns per row.
 * @param max_ln Maximum number of rows.
 * @param z_offset Z offset for spacing the grid vertically.
 * @return Pointer to the newly allocated UnitList, or NULL on allocation
 * failure.
 */
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

/**
 * @brief Removes units from the list that are no longer visible (e.g.,
 * destroyed and fallen).
 *
 * Frees memory for removed nodes and updates list pointers and length.
 *
 * @param list Pointer to the UnitList to modify.
 */
void removeUnits(UnitList *list) {
  UnitNode *node = list->head;
  while (node) {
    UnitNode *next = node->next;

    if (!node->self.render.visible) {
      if (node == list->head) {
        list->head = node->next;
      }

      if (node == list->tail) {
        list->tail = node->prev;
      }

      if (node->prev) {
        node->prev->next = node->next;
      }
      if (node->next) {
        node->next->prev = node->prev;
      }

      destroyUnitNode(node);
      list->length--;

      printf("[Units] in list: %i\n", list->length);
    }

    node = next;
  }
}

/**
 * @brief Frees all memory used by the UnitList and its nodes.
 *
 * Resets head, tail, and length to initial state.
 *
 * @param list Pointer to the UnitList to destroy.
 */
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

/**
 * @brief Inserts a new unit into the UnitList, assigning grid position and
 * links.
 *
 * @param list Pointer to the list.
 * @param unit Unit instance to insert.
 * @param max_col Grid column limit.
 * @param max_ln Grid row limit.
 * @param mid_x Center offset for positioning units horizontally.
 * @param z_offset Vertical placement offset along Z.
 */
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

/**
 * @brief Renders all units in the list and removes invisible units after
 * drawing.
 *
 * Iterates through each node in the UnitList, draws it, and cleans up destroyed
 * units.
 *
 * @param list Pointer to the UnitList.
 */
void drawUnits(UnitList *list, Camera3D *camera,
               ExplosionModelList *explosions) {
  UnitNode *node = list->head;
  for (int i = 0; i < list->length; i += 1) {
    if (!node) {
      break;
    }
    drawUnit(&node->self, camera, explosions);
    node = node->next;
  }
  removeUnits(list);
}
