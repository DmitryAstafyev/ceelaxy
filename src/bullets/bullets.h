/**
 * @file bullets.h
 * @brief Declares structures and functions for handling bullets,
 * including their movement, collision, rendering, and lifecycle management.
 */
#ifndef BULLETS_H
#define BULLETS_H

#include "../game/stat.h"
#include "../textures/textures.h"
#include "trail.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/// Maximum time (in seconds) during which a unit shows a "hit" reaction after
/// being hit by a bullet.
static const double BULLET_HIT_SEN_TIME = 0.1f;

typedef enum BulletOwner {
  BULLET_OWNER_PLAYER = 0,
  BULLET_OWNER_UNIT = 1
} BulletOwner;
/**
 * @brief Enumeration representing the direction of bullet movement.
 */
typedef enum {
  BULLET_MOVEMENT_DIRECTION_UP = 1,  ///< Moves in negative Z direction.
  BULLET_MOVEMENT_DIRECTION_DOWN = 2 ///< Moves in positive Z direction.
} BulletMovementDirection;

/**
 * @brief Stores logical parameters for a bullet, such as damage (health) and
 * energy.
 */
typedef struct {
  uint8_t health; ///< Damage or HP impact.
  uint8_t energy; ///< Energy cost or impact (if used).
} BulletParameters;

/**
 * @brief Represents the 3D position of a bullet in the world.
 */
typedef struct {
  float x, y, z;
} BulletPosition;

/**
 * @brief Describes the physical size and shape of a bullet.
 *
 * Can represent cylindrical or capsule-like shapes for rendering or collision.
 */
typedef struct {
  float by_x;          ///< Width of the bullet.
  float by_y;          ///< Height of the bullet.
  float by_z;          ///< Depth of the bullet.
  float radius_top;    ///< Optional top radius for shaping.
  float radius_bottom; ///< Optional bottom radius for shaping.
  float slices;        ///< Number of radial segments (used for mesh).
} BulletSize;

/**
 * @brief Describes the motion state of a bullet.
 */
typedef struct {
  float acceleration; ///< Acceleration along the Z axis.
  float speed;        ///< Current speed of the bullet.
  float angle; ///< Orientation angle (used for rotation or visual effects).
  uint8_t direction; ///< Movement direction (from BulletMovementDirection).
  Vector3 dir;
} BulletMovement;

/**
 * @brief Represents a single bullet instance with its full state and
 * properties.
 */
typedef struct {
  BulletMovement movement; ///< Movement-related state.
  BulletPosition position; ///< 3D position.
  BulletSize size;         ///< Shape and dimensions.
  BulletParameters params; ///< Damage and energy parameters.
  bool alive;              ///< Status flag: false if bullet is inactive.
  BulletOwner owner;
  TrailEmitter trail;
} Bullet;

/**
 * @brief Node in a doubly-linked list of bullets.
 */
typedef struct BulletNode {
  struct BulletNode *next; ///< Pointer to the next bullet in list.
  struct BulletNode *prev; ///< Pointer to the previous bullet in list.
  Bullet self;             ///< The actual bullet data.
  size_t idx;              ///< Unique identifier or spawn index.
} BulletNode;

/**
 * @brief Defines the vertical bounds within which bullets are active.
 *
 * Bullets leaving this frame may be considered out of play.
 */
typedef struct BulletAreaFrame {
  float top;    ///< Top Z boundary (usually for player bullets).
  float bottom; ///< Bottom Z boundary (usually for enemy bullets).
} BulletAreaFrame;

/**
 * @brief Represents the entire collection of active bullets in the scene.
 */
typedef struct {
  BulletNode *head;      ///< First bullet in the list.
  BulletNode *tail;      ///< Last bullet in the list.
  uint16_t length;       ///< Number of active bullets.
  size_t idx;            ///< Incremental ID for newly spawned bullets.
  double last_spawn;     ///< Time of the last bullet spawn.
  BulletAreaFrame frame; ///< Movement frame boundaries for bullets.
} BulletList;

/**
 * @brief Creates a new BulletPosition struct with the given coordinates.
 *
 * @param x X position.
 * @param y Y position.
 * @param z Z position.
 * @return Initialized BulletPosition.
 */
BulletPosition newBulletPosition(float x, float y, float z);

/**
 * @brief Creates a new BulletSize structure.
 *
 * @param by_x Width.
 * @param by_y Height.
 * @param by_z Depth.
 * @return Initialized BulletSize.
 */
BulletSize newBulletSize(float by_x, float by_y, float by_z);

/**
 * @brief Creates a new BulletParameters structure.
 *
 * @param health Damage or health effect of the bullet.
 * @param energy Energy value or cost.
 * @return Initialized BulletParameters.
 */
BulletParameters newBulletParameters(uint8_t health, uint8_t energy);

/**
 * @brief Creates and initializes a new Bullet object.
 *
 * @param direction Movement direction (up or down).
 * @param position Starting position.
 * @param size Bullet size and shape.
 * @param params Bullet damage and energy data.
 * @return Fully initialized Bullet object.
 */
Bullet newBullet(BulletMovementDirection direction, BulletPosition position,
                 BulletSize size, BulletParameters params, BulletOwner owner,
                 GameTextures *textures);

Bullet newBulletAimedAt(BulletPosition position, BulletSize size,
                        BulletParameters params, BulletOwner owner,
                        float target_x, float target_z, GameTextures *textures);
/**
 * @brief Creates a new empty BulletList with default frame bounds.
 *
 * @return Pointer to a newly allocated BulletList.
 */
BulletList *newBulletList();

/**
 * @brief Inserts a new bullet into the list.
 *
 * @param list Pointer to the BulletList.
 * @param bullet The bullet to insert.
 */
void insertBulletIntoList(BulletList *list, Bullet bullet);

/**
 * @brief Draws all active bullets in the list.
 *
 * @param list Pointer to the BulletList.
 */
void drawBullets(BulletList *list, Camera3D *camera, GameStat *stat);

/**
 * @brief Computes the bounding box for a given bullet.
 *
 * Used for collision detection with units.
 *
 * @param bullet Pointer to the bullet.
 * @return BoundingBox struct in world space.
 */
BoundingBox getBulletBoundingBox(Bullet *bullet);

/**
 * @brief Frees all memory used by the BulletList and its bullets.
 *
 * @param list Pointer to the BulletList to destroy.
 */
void destroyBulletList(BulletList *list);

void removeBullets(BulletList *list);

#endif