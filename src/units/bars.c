#include "bars.h"
#include "raylib.h"
#include "unit.h"
#include <math.h>
#include <raymath.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

void drawUnitsStateBars(UnitList *list, Camera3D *camera) {
  UnitNode *node = list->head;
  for (int i = 0; i < list->length; i += 1) {
    if (!node) {
      break;
    }
    drawUnitStateBars(&node->self, camera);
    node = node->next;
  }
}

void drawUnitStateBars(Unit *unit, Camera3D *camera) {
  if (unit == NULL || camera == NULL) {
    return;
  }

  double current = GetTime();

  bool hit = current > STATE_BAR_HIT_SEN_TIME &&
             current - unit->state.hit_time < STATE_BAR_HIT_SEN_TIME;

  if (!hit) {
    return;
  }

  BoundingBox bounding_box = getUnitBoundingBox(unit);

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

  float health_rate =
      (float)unit->state.health / (float)unit->state.init_health;
  float energy_rate =
      (float)unit->state.energy / (float)unit->state.init_energy;
  DrawRectangle(x, y, bar_width_px * health_rate, STATE_BAR_HEIGHT, RED);
  DrawRectangle(x, y - STATE_BAR_HEIGHT - 1, bar_width_px * energy_rate,
                STATE_BAR_HEIGHT, BLUE);
}
