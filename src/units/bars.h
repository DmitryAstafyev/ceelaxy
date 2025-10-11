#include "player.h"
#include "raylib.h"
#include "unit.h"
#include <raymath.h>
#include <stddef.h>
#include <stdint.h>

// Time in seconds to consider a unit "recently hit" for rendering purposes
#define STATE_BAR_HIT_SEN_TIME 1.0f
// Height of the state bar in world units
#define STATE_BAR_HEIGHT 2
// Vertical offset above the unit to position the state bar
#define STATE_BAR_Y_OFFSET 12

/**
 * @brief Draws health and energy bars above all units in the provided list.
 *
 * Each unit's health and energy are represented as colored bars positioned
 * above the unit in 3D space, oriented to face the camera.
 *
 * @param list Pointer to the UnitList containing units to draw bars for.
 * @param camera Pointer to the active Camera3D for view/projection.
 */
void drawUnitsStateBars(UnitList *list, Camera3D *camera);

/**
 * @brief Draws health and energy bars above a single unit.
 *
 * The bars are positioned above the unit in 3D space and oriented to face
 * the camera. Health is shown in green, energy in blue.
 *
 * @param unit Pointer to the Unit for which to draw the bars.
 * @param camera Pointer to the active Camera3D for view/projection.
 */
void drawUnitStateBars(Unit *unit, Camera3D *camera);

/**
 * @brief Draws health and energy bars above the player unit.
 *
 * The bars are positioned above the player in 3D space and oriented to face
 * the camera. Health is shown in green, energy in blue.
 *
 * @param player Pointer to the Player for which to draw the bars.
 * @param camera Pointer to the active Camera3D for view/projection.
 */
void drawPlayerStateBars(Player *player, Camera3D *camera);