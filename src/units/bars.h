#include "raylib.h"
#include "unit.h"
#include <raymath.h>
#include <stddef.h>
#include <stdint.h>

static const double STATE_BAR_HIT_SEN_TIME = 1.0f;
static const double STATE_BAR_HEIGHT = 2.0f;
static const double STATE_BAR_Y_OFFSET = 12.0f;

void drawUnitsStateBars(UnitList *list, Camera3D *camera);

void drawUnitStateBars(Unit *unit, Camera3D *camera);