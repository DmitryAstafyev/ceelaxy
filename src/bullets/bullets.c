/**
 * @file bullets.c
 * @brief Implements bullet behavior, including creation, movement, rendering,
 * collision detection, and lifecycle management.
 */
#include "bullets.h"
#include "../game/stat.h"
#include "../textures/textures.h"
#include "raylib.h"
#include <raymath.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Creates and initializes a new BulletAreaFrame instance.
 *
 * This function allocates and sets up a new BulletAreaFrame structure,
 * initializing its fields to default values as required.
 *
 * @return A newly created BulletAreaFrame object.
 */
BulletAreaFrame newBulletAreaFrame()
{
  BulletAreaFrame frame;
  frame.top = -60.0f;
  frame.bottom = 60.0f;
  return frame;
}

/**
 * @brief Creates and initializes a new BulletMovement structure.
 *
 * @param direction The direction in which the bullet will move.
 * @return A BulletMovement structure initialized with the specified direction.
 */
BulletMovement newBulletMovement(BulletMovementDirection direction,
                                 float acceleration, float speed)
{
  BulletMovement movement;
  movement.acceleration = acceleration;
  movement.speed = speed;
  movement.direction = direction;
  movement.dir = (Vector3){
      0.0f, 0.0f, (direction == BULLET_MOVEMENT_DIRECTION_UP) ? -1.0f : 1.0f};
  movement.angle = atan2f(movement.dir.x, movement.dir.z);
  return movement;
}

/**
 * @brief Creates a new BulletMovement struct for a bullet aimed from a starting position towards a target x-coordinate.
 *
 * @param from The starting position of the bullet.
 * @param to_x The x-coordinate of the target position the bullet should aim at.
 * @return BulletMovement The initialized movement parameters for the aimed bullet.
 */
BulletMovement newBulletAimedMovement(BulletPosition from, float to_x,
                                      float to_z, float acceleration,
                                      float speed)
{
  BulletMovement movement;
  movement.acceleration = acceleration;
  movement.speed = speed;
  float dx = to_x - from.x;
  float dz = to_z - from.z;
  float len = sqrtf(dx * dx + dz * dz);
  Vector3 dir = (len > 1e-6f) ? (Vector3){dx / len, 0.0f, dz / len}
                              : (Vector3){0.0f, 0.0f, 1.0f};
  movement.dir = dir;
  movement.angle = atan2f(dir.x, dir.z);
  movement.direction = (dir.z < 0.0f) ? BULLET_MOVEMENT_DIRECTION_UP
                                      : BULLET_MOVEMENT_DIRECTION_DOWN;
  return movement;
}

/**
 * @brief Creates a new BulletPosition with the specified coordinates.
 *
 * This function initializes and returns a BulletPosition structure
 * using the provided x, y, and z floating-point coordinates.
 *
 * @param x The x-coordinate of the bullet's position.
 * @param y The y-coordinate of the bullet's position.
 * @param z The z-coordinate of the bullet's position.
 * @return A BulletPosition structure initialized with the given coordinates.
 */
BulletPosition newBulletPosition(float x, float y, float z)
{
  BulletPosition position;
  position.x = x;
  position.y = y;
  position.z = z;
  return position;
}

/**
 * @brief Creates and initializes a new BulletSize structure.
 *
 * This function constructs a BulletSize object using the provided
 * dimensions along the x, y, and z axes.
 *
 * @param by_x The size of the bullet along the x-axis.
 * @param by_y The size of the bullet along the y-axis.
 * @param by_z The size of the bullet along the z-axis.
 * @return A BulletSize structure initialized with the specified dimensions.
 */
BulletSize newBulletSize(float by_x, float by_y, float by_z)
{
  BulletSize size;
  size.by_x = by_x;
  size.by_y = by_y;
  size.by_z = by_z;
  size.radius_top = 0.25f;
  size.radius_bottom = 0.25f;
  size.slices = 15;
  return size;
}

/**
 * @brief Creates and initializes a new BulletParameters structure.
 *
 * This function sets up a BulletParameters instance with the specified
 * health and energy values.
 *
 * @param health The initial health value for the bullet.
 * @param energy The initial energy value for the bullet.
 * @return A BulletParameters structure initialized with the given health and energy.
 */
BulletParameters newBulletParameters(float health, float energy)
{
  BulletParameters params;
  params.energy = energy;
  params.health = health;
  return params;
}

/**
 * @brief Creates a new Bullet instance with the specified movement direction and position.
 *
 * @param direction The direction in which the bullet will move.
 * @param position The initial position of the bullet.
 * @return Bullet The newly created Bullet instance.
 */
Bullet newBullet(BulletMovementDirection direction, BulletPosition position,
                 BulletSize size, BulletParameters params, BulletOwner owner,
                 float acceleration, float speed, GameTextures *textures)
{
  GameTexture *tex_fire_soft = getGameTextureById(textures, TEX_ID_FIRE_SOFT);
  if (tex_fire_soft == NULL)
  {
    TraceLog(LOG_ERROR, "Fail to find texture: %i", TEX_ID_FIRE_SOFT);
    exit(1);
  }
  Bullet bullet;
  bullet.movement = newBulletMovement(direction, acceleration, speed);
  bullet.position = position;
  bullet.params = params;
  bullet.size = size;
  bullet.alive = true;
  bullet.owner = owner;
  bullet.trail = newTrailEmitter(tex_fire_soft->tex, true);
  return bullet;
}

/**
 * @brief Creates a new Bullet instance aimed at a specific target position.
 *
 * This function initializes a Bullet object with movement parameters
 * calculated to aim from the given starting position towards the specified
 * target coordinates (target_x, target_z). The bullet is also configured
 * with size, parameters, owner, acceleration, speed, and associated textures.
 *
 * @param position The starting position of the bullet.
 * @param size The size dimensions of the bullet.
 * @param params The parameters defining the bullet's behavior (e.g., health, energy).
 * @param owner The owner of the bullet (e.g., player or unit).
 * @param target_x The x-coordinate of the target position the bullet should aim at.
 * @param target_z The z-coordinate of the target position the bullet should aim at.
 * @param acceleration The acceleration of the bullet along its movement direction.
 * @param speed The initial speed of the bullet.
 * @param textures A pointer to the GameTextures structure containing available textures.
 * @return A newly created Bullet instance aimed at the specified target.
 */
Bullet newBulletAimedAt(BulletPosition position, BulletSize size,
                        BulletParameters params, BulletOwner owner,
                        float target_x, float target_z, float acceleration,
                        float speed, GameTextures *textures)
{
  GameTexture *tex_fire_soft = getGameTextureById(textures, TEX_ID_FIRE_SOFT);
  if (!tex_fire_soft)
  {
    TraceLog(LOG_ERROR, "Fail to find texture: %i", TEX_ID_FIRE_SOFT);
    exit(1);
  }

  Bullet bullet;
  bullet.movement =
      newBulletAimedMovement(position, target_x, target_z, acceleration, speed);
  bullet.position = position;
  bullet.params = params;
  bullet.size = size;
  bullet.alive = true;
  bullet.owner = owner;
  bullet.trail = newTrailEmitter(tex_fire_soft->tex, true);
  return bullet;
}

/**
 * @brief Updates the position and state of a bullet based on its movement parameters.
 *
 * This function modifies the bullet's position according to its speed and
 * acceleration. It also checks if the bullet has moved outside the defined
 * area frame, marking it as inactive if so. If a player-owned bullet goes
 * out of bounds, it increments the miss count in the provided game statistics.
 *
 * @param bullet A pointer to the Bullet instance to be updated.
 * @param frame A pointer to the BulletAreaFrame defining the valid area for bullets.
 * @param stat A pointer to the GameStat structure for tracking hits and misses.
 */
void updateBullet(Bullet *bullet, BulletAreaFrame *frame, GameStat *stat)
{
  if (!bullet || !bullet->alive)
    return;

  bullet->movement.speed += bullet->movement.acceleration;

  bullet->position.x += bullet->movement.speed * bullet->movement.dir.x;
  bullet->position.z += bullet->movement.speed * bullet->movement.dir.z;

  if (bullet->position.z < frame->top || bullet->position.z > frame->bottom)
  {
    bullet->alive = false;
    if (bullet->owner == BULLET_OWNER_PLAYER)
    {
      addMissIntoGameStat(stat);
    }
  }
}

/**
 * @brief Renders a bullet in the 3D world and updates its trail effect.
 *
 * This function draws the bullet as a cylinder with a nose cone, based on its
 * current position and size. It also updates and renders the bullet's trail
 * effect using the provided camera for proper 3D perspective. The bullet's
 * state is updated before rendering, and if it goes out of bounds, it is marked
 * as inactive and the miss count is updated in the game statistics.
 *
 * @param bullet A pointer to the Bullet instance to be drawn.
 * @param frame A pointer to the BulletAreaFrame defining the valid area for bullets.
 * @param camera A pointer to the Camera3D used for rendering the scene.
 * @param stat A pointer to the GameStat structure for tracking hits and misses.
 */
void drawBullet(Bullet *bullet, BulletAreaFrame *frame, Camera3D *camera,
                GameStat *stat)
{
  if (!bullet || !bullet->alive || !stat || !frame || !camera)
  {
    return;
  }

  updateBullet(bullet, frame, stat);

  Vector3 center = {bullet->position.x, bullet->position.y, bullet->position.z};

  Vector3 axis = Vector3Normalize(
      (Vector3){bullet->movement.dir.x, 0.0f, bullet->movement.dir.z});

  float len = bullet->size.by_z;
  float half = len * 0.5f;

  Vector3 start = Vector3Subtract(center, Vector3Scale(axis, half));
  Vector3 end = Vector3Add(center, Vector3Scale(axis, half));

  DrawCylinderEx(start, end, bullet->size.radius_bottom,
                 bullet->size.radius_bottom, bullet->size.slices, RED);

  float nose = len * 0.35f;
  Vector3 nose_end = Vector3Add(end, Vector3Scale(axis, nose));
  DrawCylinderEx(end, nose_end, bullet->size.radius_top, 0.0f,
                 bullet->size.slices, RED);

  float dt = GetFrameTime();
  trailEmit(&bullet->trail, start, axis, dt); // направление эффекта = ось
  trailUpdate(&bullet->trail, dt);
  trailDraw(&bullet->trail, *camera);
}

/**
 * @brief Computes the axis-aligned bounding box (AABB) for a given bullet.
 *
 * This function calculates the AABB that fully contains the bullet based on
 * its position and size. The bounding box is defined by its minimum and
 * maximum corners in 3D space.
 *
 * @param bullet A pointer to the Bullet instance for which to compute the bounding box.
 * @return A BoundingBox structure representing the AABB of the bullet.
 */
BoundingBox getBulletBoundingBox(Bullet *bullet)
{
  return (BoundingBox){.min = {bullet->position.x - bullet->size.by_x / 2,
                               bullet->position.y - bullet->size.by_y / 2,
                               bullet->position.z - bullet->size.by_z / 2},
                       .max = {bullet->position.x + bullet->size.by_x / 2,
                               bullet->position.y + bullet->size.by_y / 2,
                               bullet->position.z + bullet->size.by_z / 2}};
}

/**
 * @brief Creates a new BulletNode with the specified previous node, bullet data, and index.
 *
 * This function allocates memory for a new BulletNode, initializes its fields,
 * and links it to the provided previous node. The new node's next pointer is
 * set to NULL, and it contains the given bullet data and index.
 *
 * @param prev A pointer to the previous BulletNode in the list (can be NULL).
 * @param bullet The Bullet data to be stored in the new node.
 * @param idx A unique identifier or spawn index for the new bullet.
 * @return A pointer to the newly created BulletNode, or NULL if memory allocation fails.
 */
BulletNode *newBulletNode(BulletNode *prev, Bullet bullet, size_t idx)
{
  BulletNode *node = malloc(sizeof(BulletNode));
  if (!node)
  {
    return NULL;
  }
  node->idx = idx;
  node->prev = prev;
  node->next = NULL;
  node->self = bullet;
  return node;
}

/**
 * @brief Frees the memory allocated for a BulletNode.
 *
 * This function deallocates the memory used by the specified BulletNode.
 * It does not free any linked nodes or the bullet data itself.
 *
 * @param node A pointer to the BulletNode to be destroyed.
 */
void destroyBulletNode(BulletNode *node)
{
  if (!node)
  {
    return;
  }
  free(node);
}

/**
 * @brief Creates and initializes a new BulletList instance.
 *
 * This function allocates memory for a new BulletList structure,
 * initializes its fields to default values, and returns a pointer
 * to the newly created list.
 *
 * @return A pointer to the newly created BulletList, or NULL if memory allocation fails.
 */
BulletList *newBulletList()
{
  BulletList *list = malloc(sizeof(BulletList));
  if (!list)
  {
    return NULL;
  }
  list->head = NULL;
  list->tail = NULL;
  list->length = 0;
  list->idx = 0;
  list->last_spawn = GetTime();
  list->frame = newBulletAreaFrame();
  return list;
}

/**
 * @brief Inserts a new bullet into the bullet list.
 *
 * This function creates a new BulletNode for the given bullet and
 * appends it to the end of the specified BulletList. It updates
 * the list's head, tail, length, and index accordingly.
 *
 * @param list A pointer to the BulletList where the bullet will be inserted.
 * @param bullet The Bullet instance to be added to the list.
 */
void insertBulletIntoList(BulletList *list, Bullet bullet)
{
  if (!list)
  {
    return;
  }
  list->idx += 1;
  BulletNode *node = newBulletNode(list->tail, bullet, list->idx);
  if (!node)
  {
    return;
  }
  if (!list->head)
  {
    list->head = node;
  }
  BulletNode *prev = list->tail;
  list->tail = node;
  if (prev)
  {
    prev->next = node;
  }
  list->length += 1;
  TraceLog(LOG_INFO, "[Bullets] bullet has been spawn: %f, %f, %f",
           bullet.position.x, bullet.position.y, bullet.position.z);
}

/**
 * @brief Removes inactive bullets from the bullet list.
 *
 * This function iterates through the BulletList and removes any bullets
 * that are marked as inactive (alive == false). It updates the list's
 * head, tail, and length accordingly, and frees the memory of removed nodes.
 *
 * @param list A pointer to the BulletList from which inactive bullets will be removed.
 */
void removeBullets(BulletList *list)
{
  BulletNode *node = list->head;
  while (node)
  {
    BulletNode *next = node->next;

    if (!node->self.alive)
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

      destroyBulletNode(node);
      list->length--;

      TraceLog(LOG_INFO, "[Bullets] in list: %i", list->length);
    }

    node = next;
  }
}

/**
 * @brief Updates and draws all bullets in the list, then removes inactive ones.
 *
 * This function iterates through the BulletList, updating and rendering
 * each active bullet using the provided camera and game statistics. After
 * processing all bullets, it removes any that are no longer active.
 *
 * @param list A pointer to the BulletList containing the bullets to be drawn.
 * @param camera A pointer to the Camera3D used for rendering the scene.
 * @param stat A pointer to the GameStat structure for tracking hits and misses.
 */
void drawBullets(BulletList *list, Camera3D *camera, GameStat *stat)
{
  BulletNode *node = list->head;
  // Process and draw bullets
  while (node)
  {
    drawBullet(&node->self, &list->frame, camera, stat);
    node = node->next;
  }
  // Cleanup
  removeBullets(list);
}

/**
 * @brief Destroys the entire BulletList and frees associated memory.
 *
 * This function iterates through the BulletList, freeing each BulletNode
 * and its associated resources. After all nodes are freed, it resets the
 * list's head, tail, length, and index to indicate that it is empty.
 *
 * @param list A pointer to the BulletList to be destroyed.
 */
void destroyBulletList(BulletList *list)
{
  BulletNode *node = list->head;
  while (node)
  {
    BulletNode *next = node->next;
    destroyBulletNode(node);
    node = next;
  }
  list->head = list->tail = NULL;
  list->length = 0;
  list->idx = 0;
}

// Helper function to compute collision radius of a bullet
static inline float bulletCollisionRadius(const Bullet *b)
{
  float r_shape = fmaxf(b->size.radius_top, b->size.radius_bottom);
  if (r_shape > 0.0f)
    return r_shape;
  return 0.5f * fmaxf(b->size.by_x, b->size.by_z);
}

// Check if two bullets overlap in the XZ plane
static inline bool bulletsOverlapXZ(const Bullet *a, const Bullet *b)
{
  const float dx = a->position.x - b->position.x;
  const float dz = a->position.z - b->position.z;
  const float r = bulletCollisionRadius(a) + bulletCollisionRadius(b);
  return (dx * dx + dz * dz) <= (r * r);
}

/**
 * @brief Resolves collisions between bullets in the list.
 *
 * This function checks for collisions between all pairs of bullets in the
 * BulletList. If two bullets collide (overlap in the XZ plane), both are
 * marked as inactive (alive = false). The function can be configured to
 * ignore collisions between bullets owned by the same entity.
 *
 * @param list A pointer to the BulletList containing the bullets to check for collisions.
 * @param same_owner_collides A boolean flag indicating whether bullets from the same owner should collide.
 */
void bulletsResolveMutualCollisions(BulletList *list,
                                    bool same_owner_collides)
{
  if (!list)
    return;

  size_t destroyed_pairs = 0;

  for (BulletNode *a = list->head; a; a = a->next)
  {
    Bullet *ba = &a->self;
    if (!ba->alive)
      continue;

    for (BulletNode *b = a->next; b; b = b->next)
    {
      Bullet *bb = &b->self;
      if (!bb->alive)
        continue;

      if (!same_owner_collides && ba->owner == bb->owner)
        continue;

      if (bulletsOverlapXZ(ba, bb))
      {
        ba->alive = false;
        bb->alive = false;

        destroyed_pairs++;
        break;
      }
    }
  }
  removeBullets(list);
}