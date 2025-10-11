/**
 * @file unit.h
 * @brief Declares core data structures and functions for all in-game units,
 * including players and enemies. Provides rendering, state, and list management
 * utilities.
 */
#ifndef UNIT_H
#define UNIT_H

#include "../bullets/bullets.h"
#include "../game/levels.h"
#include "../game/stat.h"
#include "../models/models.h"
#include "../movement/movement.h"
#include "../sprites/sprites.h"
#include "../textures/textures.h"
#include "explosion.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Enum representing the type of unit.
 */
typedef enum UnitType
{
  UNIT_TYPE_SOLDER = 1, /// The player-controlled unit.
  UNIT_TYPE_ENEMY = 2   /// Enemy AI-controlled unit.
} UnitType;

/**
 * @brief Holds state information for a unit, including health and energy.
 */
typedef struct UnitState
{
  uint8_t health;      /// Current health value.
  uint8_t energy;      /// Current energy or stamina.
  uint8_t init_health; /// Initianal health value.
  uint8_t init_energy; /// Initianal energy or stamina.
  double hit_time;     /// Timestamp of the last hit taken.
  double last_shoot;   /// Time of the last bullet spawn.
} UnitState;

/**
 * @brief Stores 3D position and grid mapping information for a unit.
 */
typedef struct UnitPosition
{
  float x;          /// X coordinate in world space.
  float y;          /// Y coordinate in world space.
  float z;          /// Z coordinate in world space.
  float z_max_area; /// Maximum area limit on the Z axis.
  float z_offset;   /// Offset applied on the Z axis for spacing or layering.
  uint16_t ln;      /// Logical row index in grid or formation.
  uint16_t col;     /// Logical column index in grid or formation.
  bool in_front;
} UnitPosition;

/**
 * @brief Describes the 2D size of a unit (used for collision or rendering).
 */
typedef struct UnitSize
{
  uint16_t height; /// Height of the unit.
  uint16_t width;  /// Width of the unit.
} UnitSize;

/**
 * @brief Combines rendering data and movement logic for a unit.
 */
typedef struct UnitRender
{
  UnitPosition position; /// Current 3D position and grid placement.
  UnitSize size;         /// Dimensions used for model scaling and collisions.
  MovementAction
      *action;         /// Pointer to the active movement action (can be NULL).
  uint32_t last_frame; /// Last frame number for animation or update timing.
  bool visible;        /// Visibility flag for rendering.
} UnitRender;

/**
 * @brief Represents a complete in-game unit, including type, state, rendering,
 * and model.
 */
typedef struct Unit
{
  UnitType type;                      /// Type of the unit (player or enemy).
  UnitState state;                    /// Health and energy state.
  UnitRender render;                  /// Rendering and positioning data.
  ShipModel *model;                   /// 3D model used to render this unit.
  SpriteSheetState *explosion_effect; /// Explosion animation state.
  BulletExplosion explosion_bullet;   /// Explosion effect when hit.
  SpriteSheetState *hit;              /// Hit animation state.
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
 * @brief Constructs a new Unit with specified type and model.
 *
 * Initializes internal state, position, and render configuration using
 * default values.
 *
 * @param ty Type of the unit (e.g., player or enemy).
 * @param model Pointer to the ship model used for rendering this unit.
 * @param textures Pointer to the game textures for effects.
 * @return A fully constructed Unit structure.
 */
Unit newUnit(UnitType ty, ShipModel *model, GameTextures *textures);

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
typedef struct UnitNode
{
  struct UnitNode *prev; /// Pointer to the previous node.
  struct UnitNode *next; /// Pointer to the next node.
  Unit self;             /// The actual unit instance.
} UnitNode;

/**
 * @brief A list of UnitNodes, supporting dynamic collections of units.
 */
typedef struct
{
  UnitNode *head;  /// Pointer to the first node.
  UnitNode *tail;  /// Pointer to the last node.
  uint16_t length; /// Number of nodes in the list.
} UnitList;

/**
 * @brief Creates and initializes a new UnitNode.
 *
 * @param prev Pointer to the previous node.
 * @param unit The unit instance to store in this node.
 * @param max_col Maximum column index (used for positioning).
 * @param mid_x Horizontal center of the field.
 * @param z_offset Offset along Z axis for spacing or layering.
 * @return Pointer to the newly allocated UnitNode.
 */
UnitNode *newUnitNode(UnitNode *prev, Unit unit, uint16_t max_col,
                      float mid_x, float z_offset);

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
void drawUnit(Unit *unit, Camera3D *camera, SpriteSheetList *sprites);

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
 * @param z_offset Vertical offset applied to all units along Z axis.
 * @param textures Pointer to the GameTextures for explosion effects.
 * @return Pointer to the allocated UnitList, or NULL on failure.
 */
UnitList *newUnitList(int count, ShipModel *model, uint16_t max_col,
                      float z_offset, GameTextures *textures);

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
 * @param mid_x Center X position used for horizontal centering.
 * @param z_offset Z-axis placement offset.
 */
void insertToUnitList(UnitList *list, Unit unit, uint16_t max_col,
                      float mid_x, float z_offset);

/**
 * @brief Renders all units in the list.
 *
 * @param list Pointer to the list of units.
 */
void drawUnits(UnitList *list, Camera3D *camera, SpriteSheetList *sprites);

/**
 * @brief Removes all units from the list and resets the structure.
 *
 * @param list Pointer to the UnitList to clear.
 */
void removeUnits(UnitList *list);

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
bool isUnitAbleToFire(UnitList *list, Unit *unit);

/**
 * @brief Checks for collisions between bullets and enemy units.
 *
 * If a collision is detected, unit and bullet states are updated accordingly.
 *
 * @param units Pointer to the UnitList containing targets.
 * @param bullets Pointer to the BulletList to check against.
 */
void checkBulletHitsUnits(UnitList *units, BulletList *bullets, GameStat *stat);

/**
 * @brief Spawns a bullet from the unit aimed at the specified target.
 *
 * This function checks if the unit can shoot based on its cooldown and,
 * if so, creates a new bullet directed at the target coordinates.
 *
 * @param bullets Pointer to the BulletList to insert the new bullet into.
 * @param unit Pointer to the Unit that is shooting.
 * @param target_x X coordinate of the target in world space.
 * @param target_z Z coordinate of the target in world space.
 * @param level Pointer to the current Level containing unit parameters.
 * @param textures Pointer to the GameTextures for bullet creation.
 */
void spawnUnitShoot(BulletList *bullets, Unit *unit, float target_x,
                    float target_z, Level *level, GameTextures *textures);

#endif