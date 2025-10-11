/**
 * @file player.h
 * @brief Declares the data structures and functions related to the Player
 * entity, including position, movement, rendering, and interaction logic.
 */
#ifndef PLAYER_H
#define PLAYER_H

#include "../bullets/bullets.h"
#include "../textures/textures.h"
#include "unit.h"
#include <raylib.h>
#include <stdint.h>
#include <sys/types.h>

/**
 * @brief Stores the spatial position of the player in the 3D world,
 * along with movement boundaries and Z offset for rendering or logic purposes.
 */
typedef struct
{
  float x, y, z;             /// Current position of the player.
  float max_x, max_y, max_z; /// Maximum bounds along each axis.
  float offset_z;            /// Offset applied to the Z coordinate for rendering or
                             /// bullet logic.
} PlayerPosition;

/**
 * @brief Represents the current movement state of the player, including
 * direction, acceleration, and time tracking for acceleration handling.
 */
typedef struct
{
  float acceleration;    /// Current acceleration value.
  int direction_x_key;   /// Last horizontal direction key pressed (e.g.,
                         /// KEY_LEFT, KEY_RIGHT).
  int direction_z_key;   /// Last vertical direction key pressed (e.g., KEY_UP,
                         /// KEY_DOWN).
  double last_key_press; /// Timestamp of the last movement key press.
} PlayerMovement;

/**
 * @brief Represents the player's visual orientation and tilt during movement,
 * used for dynamic feedback and smooth transitions in rendering.
 */
typedef struct
{
  float rotate_x, rotate_y,
      rotate_z; /// Current rotation angles along each axis.
  float rotate_step_x, rotate_step_y,
      rotate_step_z;      /// Rotation step values per frame.
  float angle, max_angle; /// General tilt angle and its limit (may be unused).
  float max_rotate_x, max_rotate_y,
      max_rotate_z; /// Rotation limits along each axis.
} PlayerVisualState;

/**
 * @brief Contains all visual and dynamic state information needed to render the
 * player.
 */
typedef struct PlayerRender
{
  PlayerPosition position; /// Player's current position and bounds.
  UnitSize size;           /// Size of the player's collision box or model.
  PlayerMovement movement; /// Current movement-related state.
  PlayerVisualState state; /// Rotation and tilt state for rendering feedback.
} PlayerRender;

/**
 * @brief Represents the complete Player entity, including logic, visual state,
 * model, and a reference to the bullet list.
 */
typedef struct Player
{
  UnitType type;                    /// Type of the unit (e.g., UNIT_TYPE_SOLDIER).
  UnitState state;                  /// General unit state (e.g., health, alive).
  PlayerRender render;              /// All rendering and movement-related information.
  ShipModel *model;                 /// Pointer to the 3D model used for drawing.
  BulletList *bullets;              /// Reference to the global bullet list used for firing.
  BulletExplosion explosion_bullet; /// Explosion effect when hit.
  SpriteSheetState *hit;            /// Hit animation state.
} Player;

/**
 * @brief Constructs a new PlayerRender object with initialized position, size,
 * visual state, and movement.
 *
 * @param max_x Maximum allowed X coordinate.
 * @param max_y Maximum allowed Y coordinate.
 * @param max_z Maximum allowed Z coordinate.
 * @param offset_z Offset applied on the Z axis (for bullets or rendering).
 * @return Initialized PlayerRender structure.
 */
PlayerRender newPlayerRender(float max_x, float max_y, float max_z,
                             float offset_z);

/**
 * @brief Allocates and initializes a new Player instance.
 *
 * @param max_x Maximum X-axis boundary.
 * @param max_y Maximum Y-axis boundary.
 * @param max_z Maximum Z-axis boundary.
 * @param offset_z Z-axis offset for visual or gameplay adjustment.
 * @param model Pointer to the ship's 3D model.
 * @param bullets Pointer to the global bullet list shared across units.
 * @return Pointer to a newly created Player, or NULL if allocation fails.
 */
Player *newPlayer(float max_x, float max_y, float max_z, float offset_z,
                  ShipModel *model, BulletList *bullets,
                  GameTextures *textures);
/**
 * @brief Computes the world-space bounding box of the player, accounting for
 * rotation.
 *
 * This function calculates the axis-aligned bounding box (AABB) that
 * encapsulates the player's model in world space, taking into account its
 * current position and rotation. This is useful for collision detection and
 * rendering debug information.
 *
 * @param player Pointer to the Player instance.
 * @return BoundingBox representing the player's world-space AABB.
 */
BoundingBox getPlayerBoundingBox(Player *player);

/**
 * @brief Renders the player model, hit effects, and explosion effects.
 *
 * This function updates the player's state, applies hit effects if the
 * player was recently hit, and draws the player's 3D model with appropriate
 * transformations. It also handles rendering of explosion effects and debug
 * bounding boxes if enabled.
 *
 * @param player Pointer to the Player instance to render.
 * @param level Pointer to the current Level containing player parameters.
 * @param textures Pointer to the GameTextures for rendering effects.
 * @param camera Pointer to the active Camera3D for view/projection.
 * @param sprites Pointer to the SpriteSheetList for hit animations.
 */
void drawPlayer(Player *player, Level *level, GameTextures *textures,
                Camera3D *camera, SpriteSheetList *sprites);

/**
 * @brief Frees the memory allocated for the player instance.
 *
 * This does not free the model or bullet list, as they may be shared.
 *
 * @param player Pointer to the Player to be destroyed.
 */
void destroyPlayer(Player *player);

/**
 * @brief Iterates through enemy units and makes them fire at the player if
 * aligned.
 *
 * This function checks each enemy unit in the provided list to see if the
 * player is within its firing line. If so, and if the unit is able to fire
 * based on its cooldown, it spawns a bullet aimed at the player's current
 * position.
 *
 * @param list Pointer to the UnitList containing enemy units.
 * @param camera Pointer to the active Camera3D for view/projection.
 * @param player Pointer to the Player instance.
 * @param level Pointer to the current Level containing unit parameters.
 * @param factor The horizontal range around each enemy's X position for firing.
 * @param textures Pointer to the GameTextures for bullet creation.
 */
void selectUnitsToFire(UnitList *list, Camera3D *camera, Player *player,
                       Level *level, float factor, GameTextures *textures);

/**
 * @brief Checks for collisions between player and enemy bullets, applying
 * damage if hit.
 *
 * This function iterates through the global bullet list, checking for
 * collisions with the player's bounding box. If a collision is detected,
 * the bullet is marked as inactive, and the player's health and energy are
 * reduced based on the bullet's parameters. The function also updates the
 * game statistics to record the hit.
 *
 * @param player Pointer to the Player instance.
 * @param bullets Pointer to the global BulletList containing all bullets.
 * @param stat Pointer to the GameStat for recording hits.
 */
void checkBulletHitsPlayer(Player *player, BulletList *bullets, GameStat *stat);

#endif