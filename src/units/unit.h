/**
 * @file unit.h
 * @brief Declares core data structures and functions for all in-game units,
 * including players and enemies. Provides rendering, state, and list management
 * utilities.
 */
#ifndef UNIT_H
#define UNIT_H

#include "../models/models.h"
#include "../movement/movement.h"
#include "../units/explosion.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Enum representing the type of unit.
 */
typedef enum UnitType {
  UNIT_TYPE_SOLDER = 1, ///< The player-controlled unit.
  UNIT_TYPE_ENEMY = 2   ///< Enemy AI-controlled unit.
} UnitType;

/**
 * @brief Holds state information for a unit, including health and energy.
 */
typedef struct UnitState {
  uint8_t health;  ///< Current health value.
  uint8_t energy;  ///< Current energy or stamina.
  double hit_time; ///< Timestamp of the last hit taken.
} UnitState;

/**
 * @brief Stores 3D position and grid mapping information for a unit.
 */
typedef struct UnitPosition {
  float x;          ///< X coordinate in world space.
  float y;          ///< Y coordinate in world space.
  float z;          ///< Z coordinate in world space.
  float z_max_area; ///< Maximum area limit on the Z axis.
  uint16_t ln;      ///< Logical row index in grid or formation.
  uint16_t col;     ///< Logical column index in grid or formation.
} UnitPosition;

/**
 * @brief Describes the 2D size of a unit (used for collision or rendering).
 */
typedef struct UnitSize {
  uint16_t height; ///< Height of the unit.
  uint16_t width;  ///< Width of the unit.
} UnitSize;

/**
 * @brief Combines rendering data and movement logic for a unit.
 */
typedef struct UnitRender {
  UnitPosition position; ///< Current 3D position and grid placement.
  UnitSize size;         ///< Dimensions used for model scaling and collisions.
  MovementAction
      *action;         ///< Pointer to the active movement action (can be NULL).
  uint32_t last_frame; ///< Last frame number for animation or update timing.
  bool visible;        ///< Visibility flag for rendering.
} UnitRender;

/**
 * @brief Represents a complete in-game unit, including type, state, rendering,
 * and model.
 */
typedef struct Unit {
  UnitType type;     ///< Type of the unit (player or enemy).
  UnitState state;   ///< Health and energy state.
  UnitRender render; ///< Rendering and positioning data.
  ShipModel *model;  ///< 3D model used to render this unit.
  ExplosionState *explosion;
} Unit;

/**
 * @brief Creates a new default UnitSize.
 * @return A UnitSize struct with predefined dimensions.
 */
UnitSize newUnitSize();

/**
 * @brief Creates a new default UnitPosition.
 * @return A UnitPosition struct with zeroed coordinates and indices.
 */
UnitPosition newUnitPosition();

/**
 * @brief Creates a new UnitRender based on the given position.
 * @param position Initial position of the unit.
 * @return A fully initialized UnitRender structure.
 */
UnitRender newUnitRender(UnitPosition position);

/**
 * @brief Initializes and returns a new UnitState with default values.
 * @return A UnitState struct with full health and default energy.
 */
UnitState newUnitState();

/**
 * @brief Constructs a new Unit with the given type and model.
 *
 * @param ty Unit type (e.g., player or enemy).
 * @param model Pointer to the model used for rendering the unit.
 * @return A fully initialized Unit structure.
 */
Unit newUnit(UnitType ty, ShipModel *model);

/**
 * @brief Computes the bounding box of the given unit for collision or
 * debugging.
 *
 * @param unit Pointer to the unit.
 * @return BoundingBox structure that encloses the unit.
 */
BoundingBox getUnitBoundingBox(Unit *unit);

/**
 * @brief Doubly-linked list node for storing a single unit.
 */
typedef struct UnitNode {
  struct UnitNode *prev; ///< Pointer to the previous node.
  struct UnitNode *next; ///< Pointer to the next node.
  Unit self;             ///< The actual unit instance.
} UnitNode;

/**
 * @brief A list of UnitNodes, supporting dynamic collections of units.
 */
typedef struct {
  UnitNode *head;  ///< Pointer to the first node.
  UnitNode *tail;  ///< Pointer to the last node.
  uint16_t length; ///< Number of nodes in the list.
} UnitList;

/**
 * @brief Creates and initializes a new UnitNode.
 *
 * @param prev Pointer to the previous node.
 * @param unit The unit instance to store in this node.
 * @param max_col Maximum column index (used for positioning).
 * @param max_ln Maximum line index (used for positioning).
 * @param mid_x Horizontal center of the field.
 * @param z_offset Offset along Z axis for spacing or layering.
 * @return Pointer to the newly allocated UnitNode.
 */
UnitNode *newUnitNode(UnitNode *prev, Unit unit, int max_col, int max_ln,
                      float mid_x, float z_offset);

/**
 * @brief Renders a single unit using its model and transform data.
 *
 * @param unit Pointer to the unit to draw.
 */
void drawUnit(Unit *unit, Camera3D *camera, ExplosionModelList *explosions);

/**
 * @brief Creates a new UnitList and fills it with a specified number of units.
 *
 * @param count Number of units to initialize.
 * @param model Pointer to the model used for all units.
 * @param max_col Maximum grid columns for layout.
 * @param max_ln Maximum grid lines for layout.
 * @param z_offset Z-axis spacing or offset for placement.
 * @return Pointer to the newly allocated UnitList.
 */
UnitList *newUnitList(int count, ShipModel *model, int max_col, int max_ln,
                      float z_offset);

/**
 * @brief Frees all memory used by the unit list and its nodes.
 *
 * @param list Pointer to the UnitList to destroy.
 */
void destroyUnitList(UnitList *list);

/**
 * @brief Inserts a new unit into the end of the UnitList.
 *
 * @param list Pointer to the list to insert into.
 * @param unit Unit to be inserted.
 * @param max_col Grid column limit (used for positioning).
 * @param max_ln Grid line limit.
 * @param mid_x Center X position used for horizontal centering.
 * @param z_offset Z-axis placement offset.
 */
void insertToUnitList(UnitList *list, Unit unit, int max_col, int max_ln,
                      float mid_x, float z_offset);

/**
 * @brief Renders all units in the list.
 *
 * @param list Pointer to the list of units.
 */
void drawUnits(UnitList *list, Camera3D *camera,
               ExplosionModelList *explosions);

/**
 * @brief Removes all units from the list and resets the structure.
 *
 * @param list Pointer to the UnitList to clear.
 */
void removeUnits(UnitList *list);

#endif