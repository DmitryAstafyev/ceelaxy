/**
 * @file bullets.c
 * @brief Implements bullet behavior, including creation, movement, rendering,
 * collision detection, and lifecycle management.
 */
#include "bullets.h"
#include "../units/unit.h"
#include "raylib.h"
#include <raymath.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Creates a new bullet area frame defining valid Z boundaries for
 * bullets.
 *
 * @return BulletAreaFrame with default top/bottom Z range.
 */
BulletAreaFrame newBulletAreaFrame() {
  BulletAreaFrame frame;
  frame.top = -60.0f;
  frame.bottom = 60.0f;
  return frame;
}

/**
 * @brief Initializes a BulletMovement struct with default speed and
 * acceleration.
 *
 * @param direction Direction of bullet travel (up or down).
 * @return A new BulletMovement object.
 */
BulletMovement newBulletMovement(BulletMovementDirection direction) {
  BulletMovement movement;
  movement.acceleration = 0.1f;
  movement.angle = 0.0f;
  movement.direction = direction;
  movement.speed = 1.0f;
  return movement;
}

/**
 * @brief Creates a new BulletPosition with specified coordinates.
 *
 * @param x X coordinate.
 * @param y Y coordinate.
 * @param z Z coordinate.
 * @return A BulletPosition struct.
 */
BulletPosition newBulletPosition(float x, float y, float z) {
  BulletPosition position;
  position.x = x;
  position.y = y;
  position.z = z;
  return position;
}

/**
 * @brief Creates a new BulletSize with default shape parameters.
 *
 * @param by_x Width of the bullet.
 * @param by_y Height of the bullet.
 * @param by_z Depth of the bullet.
 * @return A BulletSize struct with top/bottom radii and slices set.
 */
BulletSize newBulletSize(float by_x, float by_y, float by_z) {
  BulletSize size;
  size.by_x = by_x;
  size.by_y = by_y;
  size.by_z = by_z;
  size.radius_top = 0.1f;
  size.radius_bottom = 0.5f;
  size.slices = 5;
  return size;
}

/**
 * @brief Creates bullet parameters that define its effect on health and energy.
 *
 * @param health Health impact on hit.
 * @param energy Energy impact on hit.
 * @return A BulletParameters struct.
 */
BulletParameters newBulletParameters(uint8_t health, uint8_t energy) {
  BulletParameters params;
  params.energy = energy;
  params.health = health;
  return params;
}

/**
 * @brief Creates a fully initialized Bullet object.
 *
 * @param direction Movement direction.
 * @param position Starting position.
 * @param size Physical size of the bullet.
 * @param params Damage and energy properties.
 * @return A Bullet instance.
 */
Bullet newBullet(BulletMovementDirection direction, BulletPosition position,
                 BulletSize size, BulletParameters params) {
  Bullet bullet;
  bullet.movement = newBulletMovement(direction);
  bullet.position = position;
  bullet.params = params;
  bullet.size = size;
  bullet.alive = true;
  return bullet;
}

/**
 * @brief Updates bullet position and checks whether it exited the valid frame.
 *
 * @param bullet Pointer to the bullet.
 * @param frame Pointer to the frame defining allowed Z range.
 */
void updateBullet(Bullet *bullet, BulletAreaFrame *frame) {
  if (!bullet) {
    return;
  }
  int direction =
      bullet->movement.direction == BULLET_MOVEMENT_DIRECTION_UP ? -1 : 1;
  bullet->position.z += bullet->movement.speed * direction;
  if ((bullet->movement.direction == BULLET_MOVEMENT_DIRECTION_UP &&
       bullet->position.z < frame->top) ||
      (bullet->movement.direction == BULLET_MOVEMENT_DIRECTION_DOWN &&
       bullet->position.z > frame->bottom)) {
    bullet->alive = false;
  }
}

/**
 * @brief Draws a bullet as a 3D cylinder and updates its position.
 *
 * @param bullet Pointer to the bullet.
 * @param frame Pointer to the bullet area frame.
 */
void drawBullet(Bullet *bullet, BulletAreaFrame *frame) {
  if (!bullet) {
    return;
  }
  updateBullet(bullet, frame);
  DrawCylinder(
      (Vector3){bullet->position.x, bullet->position.y, bullet->position.z},
      bullet->size.radius_top, bullet->size.radius_bottom, bullet->size.by_y,
      bullet->size.slices, RED);
}

/**
 * @brief Computes the bounding box for a given bullet.
 *
 * Used for collision detection with units.
 *
 * @param bullet Pointer to the bullet.
 * @return BoundingBox struct in world space.
 */
BoundingBox getBulletBoundingBox(Bullet *bullet) {
  return (BoundingBox){.min = {bullet->position.x - bullet->size.by_x / 2,
                               bullet->position.y - bullet->size.by_y / 2,
                               bullet->position.z - bullet->size.by_z / 2},
                       .max = {bullet->position.x + bullet->size.by_x / 2,
                               bullet->position.y + bullet->size.by_y / 2,
                               bullet->position.z + bullet->size.by_z / 2}};
}

/**
 * @brief Creates and initializes a new BulletNode.
 *
 * @param prev Pointer to the previous node.
 * @param bullet Bullet to be stored in the node.
 * @param idx Unique index or identifier.
 * @return Pointer to the new BulletNode or NULL on failure.
 */
BulletNode *newBulletNode(BulletNode *prev, Bullet bullet, size_t idx) {
  BulletNode *node = malloc(sizeof(BulletNode));
  if (!node) {
    return NULL;
  }
  node->idx = idx;
  node->prev = prev;
  node->next = NULL;
  node->self = bullet;
  return node;
}

/**
 * @brief Frees memory allocated for a single BulletNode.
 *
 * @param node Pointer to the node to free.
 */
void destroyBulletNode(BulletNode *node) {
  if (!node) {
    return;
  }
  free(node);
}

/**
 * @brief Allocates and initializes a new BulletList.
 *
 * @return Pointer to the new BulletList.
 */
BulletList *newBulletList() {
  BulletList *list = malloc(sizeof(BulletList));
  if (!list) {
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
 * @param list Pointer to the bullet list.
 * @param bullet Bullet to insert.
 */
void insertBulletIntoList(BulletList *list, Bullet bullet) {
  if (!list) {
    return;
  }
  list->idx += 1;
  BulletNode *node = newBulletNode(list->tail, bullet, list->idx);
  if (!node) {
    return;
  }
  if (!list->head) {
    list->head = node;
  }
  BulletNode *prev = list->tail;
  list->tail = node;
  if (prev) {
    prev->next = node;
  }
  list->length += 1;
  printf("[Bullets] bullet has been spawn: %f, %f, %f\n", bullet.position.x,
         bullet.position.y, bullet.position.z);
}

/**
 * @brief Removes all inactive (dead) bullets from the list.
 *
 * Updates list links and decrements length accordingly.
 *
 * @param list Pointer to the bullet list.
 */
void removeBullets(BulletList *list) {
  BulletNode *node = list->head;
  while (node) {
    BulletNode *next = node->next;

    if (!node->self.alive) {
      if (node == list->head) {
        list->head = node->next;
      }

      if (node == list->tail) {
        list->tail = node->prev;
      }

      if (node->prev) {
        node->prev->next = node->next;
      }
      if (node->next) {
        node->next->prev = node->prev;
      }

      destroyBulletNode(node);
      list->length--;

      printf("[Bullets] in list: %i\n", list->length);
    }

    node = next;
  }
}

/**
 * @brief Draws all bullets in the list and updates their positions.
 *
 * Also removes any bullets that have become inactive.
 *
 * @param list Pointer to the bullet list.
 */
void drawBullets(BulletList *list) {
  BulletNode *node = list->head;
  // Process and draw bullets
  while (node) {
    drawBullet(&node->self, &list->frame);
    node = node->next;
  }
  // Cleanup
  removeBullets(list);
}

/**
 * @brief Checks if any bullet in the list has collided with the given unit.
 *
 * On collision, bullet is deactivated and unit health/energy is reduced.
 *
 * @param unit Pointer to the target unit.
 * @param bullets Pointer to the bullet list.
 */
void checkBulletHitsUnit(Unit *unit, BulletList *bullets) {
  if (!unit || !bullets)
    return;

  BoundingBox unitBox = getUnitBoundingBox(unit);

  BulletNode *node = bullets->head;
  while (node) {
    Bullet *bullet = &node->self;
    if (!bullet->alive) {
      node = node->next;
      continue;
    }

    BoundingBox bulletBox = getBulletBoundingBox(bullet);

    if (CheckCollisionBoxes(unitBox, bulletBox)) {
      bullet->alive = false;
      if (unit->state.health > 0) {
        unit->state.health -= bullet->params.health;
      }
      if (unit->state.energy > 0) {
        unit->state.energy -= bullet->params.energy;
      }
      unit->state.hit_time = GetTime();
      printf("[Bullets] HIT! health = %u\n", unit->state.health);
    }

    node = node->next;
  }
}

/**
 * @brief Applies bullet collision checks to all units in the list.
 *
 * @param units Pointer to the unit list.
 * @param bullets Pointer to the bullet list.
 */
void checkBulletHitsUnits(UnitList *units, BulletList *bullets) {
  if (!units || !bullets) {
    return;
  }
  UnitNode *node = units->head;
  while (node) {
    checkBulletHitsUnit(&node->self, bullets);
    node = node->next;
  }
  removeBullets(bullets);
}

/**
 * @brief Frees all memory used by the bullet list and its nodes.
 *
 * @param list Pointer to the bullet list.
 */
void destroyBulletList(BulletList *list) {
  BulletNode *node = list->head;
  while (node) {
    BulletNode *next = node->next;
    destroyBulletNode(node);
    node = next;
  }
  list->head = list->tail = NULL;
  list->length = 0;
  list->idx = 0;
}