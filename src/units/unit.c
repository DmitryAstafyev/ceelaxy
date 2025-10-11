/**
 * @file unit.c
 * @brief Implements unit-related creation and initialization logic,
 * including size, position, state, and rendering configuration.
 */
#include "unit.h"
#include "../bullets/bullets.h"
#include "../game/levels.h"
#include "../game/stat.h"
#include "../models/models.h"
#include "../movement/movement.h"
#include "../sprites/sprites.h"
#include "../textures/textures.h"
#include "../utils/debug.h"
#include "raylib.h"
#include <math.h>
#include <raymath.h>
#include <stdbool.h>
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
UnitSize newUnitSize()
{
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
UnitPosition newUnitPosition()
{
  UnitPosition position;
  position.x = 0.0f;
  position.y = 0.0f;
  position.z = 0.0f;
  position.z_max_area = 300.0f;
  position.z_offset = -30.0f;
  position.in_front = false;
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
UnitRender newUnitRender(UnitPosition position)
{
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
UnitState newUnitState()
{
  UnitState state;
  state.health = DEFAULT_UNIT_HEALTH;
  state.energy = DEFAULT_UNIT_ENERGY;
  state.init_health = DEFAULT_UNIT_HEALTH;
  state.init_energy = DEFAULT_UNIT_ENERGY;
  state.hit_time = 0.0f;
  state.last_shoot = 0.0f;
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
Unit newUnit(UnitType ty, ShipModel *model, GameTextures *textures)
{
  GameTexture *tex_fire_soft = getGameTextureById(textures, TEX_ID_FIRE_SOFT);
  if (tex_fire_soft == NULL)
  {
    TraceLog(LOG_ERROR, "Fail to find texture: %i", TEX_ID_FIRE_SOFT);
    exit(1);
  }
  GameTexture *tex_smoke_soft = getGameTextureById(textures, TEX_ID_SMOKE_SOFT);
  if (tex_smoke_soft == NULL)
  {
    TraceLog(LOG_ERROR, "Fail to find texture: %i", TEX_ID_SMOKE_SOFT);
    exit(1);
  }
  GameTexture *tex_glow = getGameTextureById(textures, TEX_ID_GLOW);
  if (tex_glow == NULL)
  {
    TraceLog(LOG_ERROR, "Fail to find texture: %i", TEX_ID_GLOW);
    exit(1);
  }
  Unit unit;
  unit.type = ty;
  unit.state = newUnitState();
  unit.render = newUnitRender(newUnitPosition());
  unit.model = model;
  unit.explosion_effect = NULL;
  unit.hit = NULL;
  unit.explosion_bullet = newBulletExplosion(
      tex_fire_soft->tex, tex_smoke_soft->tex, tex_glow->tex);
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
                      float mid_x, float z_offset)
{
  UnitNode *node = malloc(sizeof(UnitNode));
  if (!node)
  {
    return NULL;
  }
  int prev_col, prev_ln;
  if (prev)
  {
    unit.render.position.col = prev->self.render.position.col + 1;
    prev_ln = prev->self.render.position.ln;
  }
  else
  {
    unit.render.position.col = 0;
    prev_ln = 0;
  }
  if (unit.render.position.col == max_col)
  {
    unit.render.position.ln = prev_ln + 1;
    unit.render.position.col = 0;
  }
  else
  {
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
void destroyUnitNode(UnitNode *node)
{
  if (node != NULL)
  {
    destroyMovementAction(node->self.render.action);
    if (node->self.explosion_effect)
    {
      destroySpriteSheetState(node->self.explosion_effect);
    }
    if (node->self.hit)
    {
      destroySpriteSheetState(node->self.hit);
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
void updateDestroyedUnitFall(Unit *unit, float deltaTime)
{
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

  if (action->angle > 360.0f)
  {
    action->angle -= 360.0f;
  }
  if (fabsf(position->z + position->z_offset + action->z) >
      fabsf(position->z_max_area))
  {
    unit->render.visible = false;
  }
}

/**
 * @brief Renders the unit's model, explosion effects, and hit animations.
 *
 * Handles the drawing of the unit's 3D model, applying hit effects and
 * explosion animations as necessary. Also manages movement updates and debug
 * bounding box rendering.
 *
 * @param unit Pointer to the Unit to draw.
 * @param camera Pointer to the active Camera3D for view/projection.
 * @param sprites Pointer to the SpriteSheetList containing explosion models.
 */
void drawUnit(Unit *unit, Camera3D *camera, SpriteSheetList *sprites)
{
  if (!unit)
  {
    return;
  }

  UnitPosition *position = &unit->render.position;
  if (unit->render.position.z_offset < 0)
  {
    float step = 0.1f + (0.2f - 0.1f) * ((float)rand() / (float)RAND_MAX);
    unit->render.position.z_offset += step;
    if (unit->render.position.z_offset > 0)
    {
      unit->render.position.z_offset = 0;
    }
  }
  double current = GetTime();
  bool hit = current > BULLET_HIT_SEN_TIME &&
             current - unit->state.hit_time < BULLET_HIT_SEN_TIME;
  MovementAction *action = unit->render.action;
  float dt = GetFrameTime();
  Vector3 origin =
      (Vector3){position->x + action->x, position->y + action->y + 2.0f,
                position->z + position->z_offset + action->z + 2.0f};
  if (hit)
  {
    setShipModelColor(unit->model, RED);
    if (!unit->hit)
    {
      unit->hit = newSpriteSheetState(&sprites->tail->self, 1, 3.0f, 0.1f);
    }
    dropSpriteSheetState(unit->hit);
    bulletExplosionSpawnAt(&unit->explosion_bullet, origin, camera);
  }
  bulletExplosionUpdate(&unit->explosion_bullet,
                        (Vector3){position->x + action->x,
                                  position->y + action->y,
                                  position->z + position->z_offset + action->z},
                        dt, camera);
  bulletExplosionDraw(&unit->explosion_bullet, *camera);
  drawSpriteSheetState(unit->hit, *camera, origin);
  if (unit->state.health == 0)
  {
    updateDestroyedUnitFall(unit, GetFrameTime());
    if (!unit->explosion_effect)
    {
      unit->explosion_effect =
          newSpriteSheetState(&sprites->head->self, 3, 20.0f, 1.0f);
    }
    if (unit->explosion_effect)
    {
      drawSpriteSheetState(
          unit->explosion_effect, *camera,
          (Vector3){position->x + action->x, position->y + action->y,
                    position->z + position->z_offset + action->z});
    }
  }
  else
  {
    iterateMovementAction(action, (float)unit->state.energy /
                                      (float)unit->state.init_energy);
  }
  DrawModelEx(unit->model->model,
              (Vector3){position->x + action->x, position->y + action->y,
                        position->z + position->z_offset + action->z},
              (Vector3){action->rotate_x, action->rotate_y, action->rotate_z},
              action->angle, (Vector3){1, 1, 1}, hit ? RED : WHITE);
  if (hit)
  {
    setShipModelColor(unit->model, WHITE);
  }
  if (is_debug_mode && unit->model->box_model)
  {
    DrawModelEx(*unit->model->box_model,
                (Vector3){position->x + action->x, position->y + action->y,
                          position->z + position->z_offset + action->z},
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
BoundingBox getUnitBoundingBox(Unit *unit)
{
  ShipBoundingBox *box = &unit->model->box;
  UnitRender *render = &unit->render;
  MovementAction *action = render->action;

  float x = render->position.x + (action ? action->x : 0);
  float y = render->position.y + (action ? action->y : 0);
  float z =
      render->position.z + (action ? action->z : 0) + render->position.z_offset;

  Vector3 position = {x, y, z};

  BoundingBox local = {.min = {-box->by_x / 2, -box->by_y / 2, -box->by_z / 2},
                       .max = {box->by_x / 2, box->by_y / 2, box->by_z / 2}};

  Matrix transform = MatrixTranslate(position.x, position.y, position.z);
  if (action)
  {
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

  for (int i = 1; i < 8; i++)
  {
    Vector3 transformed = Vector3Transform(corners[i], transform);
    world.min = Vector3Min(world.min, transformed);
    world.max = Vector3Max(world.max, transformed);
  }

  return world;
}

/**
 * @brief Creates and populates a UnitList with a specified number of enemy
 * units.
 *
 * Units are arranged in a grid formation based on max columns and lines,
 * centered around the origin.
 *
 * @param count Number of units to create.
 * @param model Pointer to the ship model used for all units.
 * @param max_col Maximum columns in the formation grid.
 * @param max_ln Maximum lines (rows) in the formation grid.
 * @param z_offset Vertical offset applied to all units along Z axis.
 * @param textures Pointer to the GameTextures for explosion effects.
 * @return Pointer to the allocated UnitList, or NULL on failure.
 */
UnitList *newUnitList(int count, ShipModel *model, int max_col, int max_ln,
                      float z_offset, GameTextures *textures)
{
  UnitList *units = malloc(sizeof(UnitList));
  if (!units)
  {
    return NULL;
  }
  units->length = 0;
  units->head = NULL;
  units->tail = NULL;
  float unit_full_width = DEFAULT_UNIT_WIDTH + UNIT_SPACE_HORIZONTAL;
  float unit_full_height = DEFAULT_UNIT_HEIGHT + UNIT_SPACE_VERTICAL;

  float mid_x = (unit_full_width * max_col) / 2.0f - unit_full_width / 2.0f;
  for (int i = count - 1; i >= 0; i -= 1)
  {
    insertToUnitList(units, newUnit(UNIT_TYPE_ENEMY, model, textures), max_col,
                     max_ln, mid_x, z_offset);
    TraceLog(LOG_INFO, "[Units] Added unit %i", i);
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
void removeUnits(UnitList *list)
{
  UnitNode *node = list->head;
  while (node)
  {
    UnitNode *next = node->next;

    if (!node->self.render.visible)
    {
      if (node == list->head)
      {
        list->head = node->next;
      }

      if (node == list->tail)
      {
        list->tail = node->prev;
      }

      if (node->prev)
      {
        node->prev->next = node->next;
      }
      if (node->next)
      {
        node->next->prev = node->prev;
      }

      destroyUnitNode(node);
      list->length--;

      TraceLog(LOG_INFO, "[Units] in list: %i", list->length);
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
void destroyUnitList(UnitList *list)
{
  UnitNode *node = list->head;
  while (node)
  {
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
                      float mid_x, float z_offset)
{
  UnitNode *node =
      newUnitNode(list->tail, unit, max_col, max_ln, mid_x, z_offset);
  if (!node)
  {
    return;
  }
  if (list->length == 0)
  {
    list->head = list->tail = node;
  }
  else
  {
    list->tail->next = node;
    node->prev = list->tail;
    list->tail = node;
  }
  list->length += 1;
}

/**
 * @brief Draws all units in the list and removes any that are no longer
 * visible.
 *
 * @param list Pointer to the UnitList to draw.
 * @param camera Pointer to the active Camera3D for view/projection.
 * @param sprites Pointer to the SpriteSheetList containing explosion models.
 */
void drawUnits(UnitList *list, Camera3D *camera, SpriteSheetList *sprites)
{
  UnitNode *node = list->head;
  for (int i = 0; i < list->length; i += 1)
  {
    if (!node)
    {
      break;
    }
    drawUnit(&node->self, camera, sprites);
    node = node->next;
  }
  removeUnits(list);
}

/**
 * @brief Determines if a unit is able to fire based on its position in the
 * formation.
 *
 * A unit can fire if it is in front of all other units in its column.
 *
 * @param list Pointer to the UnitList containing all units.
 * @param unit Pointer to the specific unit to check.
 * @return true if the unit can fire, false otherwise.
 */
bool isUnitAbleToFire(UnitList *list, Unit *unit)
{
  if (!list || !unit)
  {
    return false;
  }
  if (unit->render.position.in_front)
  {
    return true;
  }
  UnitNode *node = list->head;
  for (int i = 0; i < list->length; i += 1)
  {
    if (!node)
    {
      break;
    }
    if (unit->render.position.col == node->self.render.position.col &&
        unit->render.position.ln < node->self.render.position.ln)
    {
      return false;
    }
    node = node->next;
  }
  unit->render.position.in_front = true;
  return true;
}

/**
 * @brief Checks for collisions between bullets and a single unit.
 *
 * If a bullet hits the unit, it reduces health and energy, marks the bullet
 * as inactive, and updates the game statistics.
 *
 * @param unit Pointer to the unit to check for hits.
 * @param bullets Pointer to the BulletList containing all active bullets.
 * @param stat Pointer to the GameStat structure to update on hits.
 */
void checkBulletHitsUnit(Unit *unit, BulletList *bullets, GameStat *stat)
{
  if (!unit || !bullets)
    return;

  BoundingBox unitBox = getUnitBoundingBox(unit);

  BulletNode *node = bullets->head;
  while (node)
  {
    Bullet *bullet = &node->self;
    if (!bullet->alive || bullet->owner != BULLET_OWNER_PLAYER)
    {
      node = node->next;
      continue;
    }

    BoundingBox bulletBox = getBulletBoundingBox(bullet);

    if (CheckCollisionBoxes(unitBox, bulletBox))
    {
      bullet->alive = false;
      if (unit->state.health > 0)
      {
        unit->state.health =
            (unit->state.health > bullet->params.health)
                ? (uint8_t)(unit->state.health - bullet->params.health)
                : 0u;
      }
      if (unit->state.energy > 0)
      {
        unit->state.energy =
            (unit->state.energy > bullet->params.energy)
                ? (uint8_t)(unit->state.energy - bullet->params.energy)
                : 0u;
      }
      unit->state.hit_time = GetTime();
      addHitIntoGameStat(stat);
      TraceLog(LOG_INFO, "[Units] HIT! health = %u", unit->state.health);
    }

    node = node->next;
  }
}

/**
 * @brief Checks for bullet collisions against all units in the list.
 *
 * Iterates through each unit and checks for hits with bullets. Updates game
 * statistics accordingly and removes inactive bullets after processing.
 *
 * @param units Pointer to the UnitList containing all units.
 * @param bullets Pointer to the BulletList containing all active bullets.
 * @param stat Pointer to the GameStat structure to update on hits.
 */
void checkBulletHitsUnits(UnitList *units, BulletList *bullets,
                          GameStat *stat)
{
  if (!units || !bullets || !stat)
  {
    return;
  }
  UnitNode *node = units->head;
  while (node)
  {
    checkBulletHitsUnit(&node->self, bullets, stat);
    node = node->next;
  }
  removeBullets(bullets);
}

/**
 * @brief Spawns a bullet from the unit towards a target position if enough
 * time has elapsed since the last shot.
 *
 * @param bullets Pointer to the BulletList to add the new bullet to.
 * @param unit Pointer to the Unit that is shooting.
 * @param target_x X coordinate of the target position.
 * @param target_z Z coordinate of the target position.
 * @param level Pointer to the current Level for bullet parameters.
 * @param textures Pointer to GameTextures for bullet appearance.
 */
void spawnUnitShoot(BulletList *bullets, Unit *unit, float target_x,
                    float target_z, Level *level, GameTextures *textures)
{
  double current_time = GetTime();
  double elapsed_last_bullet_spawn = current_time - unit->state.last_shoot;

  if (elapsed_last_bullet_spawn > level->units.bullet_delay_spawn)
  {
    Bullet bullet = newBulletAimedAt(
        newBulletPosition(unit->render.position.x, unit->render.position.y,
                          unit->render.position.z +
                              unit->render.position.z_offset),
        newBulletSize(0.25f, .25f, 2.0f),
        newBulletParameters(level->units.damage_life,
                            level->units.damage_energy),
        BULLET_OWNER_UNIT, target_x, target_z, level->units.bullet_acceleration,
        level->units.bullet_init_speed, textures);
    insertBulletIntoList(bullets, bullet);
    unit->state.last_shoot = current_time;
  }
}