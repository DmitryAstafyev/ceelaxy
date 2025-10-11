#include "bars.h"
#include "player.h"
#include "raylib.h"
#include "unit.h"
#include <math.h>
#include <raymath.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * @brief Draws health and energy bars above all units in the provided list.
 *
 * Each unit's health and energy are represented as colored bars positioned
 * above the unit in 3D space, oriented to face the camera.
 *
 * @param list Pointer to the UnitList containing units to draw bars for.
 * @param camera Pointer to the active Camera3D for view/projection.
 */
void drawUnitsStateBars(UnitList *list, Camera3D *camera)
{
  UnitNode *node = list->head;
  for (int i = 0; i < list->length; i += 1)
  {
    if (!node)
    {
      break;
    }
    drawUnitStateBars(&node->self, camera);
    node = node->next;
  }
}

/**
 * @brief Helper function to draw health and energy bars above a bounding box.
 *
 * The bars are positioned above the bounding box in 3D space and oriented
 * to face the camera. Health is shown in red, energy in blue.
 *
 * @param bounding_box The bounding box above which to draw the bars.
 * @param state Pointer to the UnitState containing health and energy info.
 * @param camera Pointer to the active Camera3D for view/projection.
 */
void drawStateBars(BoundingBox bounding_box, UnitState *state,
                   Camera3D *camera)
{
  if (state == NULL || camera == NULL)
  {
    return;
  }

  Vector3 worldCenter = {(bounding_box.min.x + bounding_box.max.x) * 0.5f,
                         bounding_box.max.y,
                         (bounding_box.min.z + bounding_box.max.z) * 0.5f};

  Vector2 screenTop = GetWorldToScreen(worldCenter, *camera);

  Vector3 worldLeft = {bounding_box.min.x, bounding_box.max.y, worldCenter.z};
  Vector3 worldRight = {bounding_box.max.x, bounding_box.max.y, worldCenter.z};
  Vector2 screenLeft = GetWorldToScreen(worldLeft, *camera);
  Vector2 screenRight = GetWorldToScreen(worldRight, *camera);
  float bar_width_px = fabsf(screenRight.x - screenLeft.x);

  float x = screenTop.x - bar_width_px / 2.0f;
  float y = screenTop.y - STATE_BAR_Y_OFFSET;

  float health_rate = (float)state->health / (float)state->init_health;
  float energy_rate = (float)state->energy / (float)state->init_energy;
  DrawRectangle(x, y, bar_width_px * health_rate, STATE_BAR_HEIGHT, RED);
  DrawRectangle(x, y - STATE_BAR_HEIGHT - 1, bar_width_px * energy_rate,
                STATE_BAR_HEIGHT, BLUE);
}

/**
 * @brief Draws health and energy bars above a single unit.
 *
 * The bars are positioned above the unit in 3D space and oriented to face
 * the camera. Health is shown in green, energy in blue.
 *
 * @param unit Pointer to the Unit for which to draw the bars.
 * @param camera Pointer to the active Camera3D for view/projection.
 */
void drawUnitStateBars(Unit *unit, Camera3D *camera)
{
  if (unit == NULL || camera == NULL)
  {
    return;
  }

  double current = GetTime();

  bool hit = current > STATE_BAR_HIT_SEN_TIME &&
             current - unit->state.hit_time < STATE_BAR_HIT_SEN_TIME;

  if (!hit)
  {
    return;
  }

  BoundingBox bounding_box = getUnitBoundingBox(unit);
  drawStateBars(bounding_box, &unit->state, camera);
}

void drawPlayerStateBars(Player *player, Camera3D *camera)
{
  if (player == NULL || camera == NULL)
  {
    return;
  }

  double current = GetTime();

  bool hit = current > STATE_BAR_HIT_SEN_TIME &&
             current - player->state.hit_time < STATE_BAR_HIT_SEN_TIME;

  if (!hit)
  {
    return;
  }

  BoundingBox bounding_box = getPlayerBoundingBox(player);

  drawStateBars(bounding_box, &player->state, camera);
}
