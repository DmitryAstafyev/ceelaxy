#include "bullets.h"
#include "raylib.h" // For Model, Texture2D, Shader
#include <raymath.h>
#include <stddef.h>
#include <stdlib.h>

BulletMovement newBulletMovement(BulletMovementDirection direction) {
  BulletMovement movement;
  movement.acceleration = 0.1f;
  movement.angle = 0.0f;
  movement.direction = direction;
  movement.speed = 1.0f;
  return movement;
}

BulletPosition newBulletPosition(float x, float y, float z) {
  BulletPosition position;
  position.x = x;
  position.y = y;
  position.z = z;
  return position;
}

BulletSize newBulletSize(float by_x, float by_y, float by_z) {
  BulletSize size;
  size.by_x = by_x;
  size.by_y = by_y;
  size.by_z = by_z;
  return size;
}

Bullet newBullet(BulletMovementDirection direction, BulletPosition position,
                 BulletSize size) {
  Bullet bullet;
  bullet.movement = newBulletMovement(direction);
  bullet.position = position;
  bullet.size = size;
  return bullet;
}

void updateBullet(Bullet *bullet) {
  if (!bullet) {
    return;
  }
  int direction =
      bullet->movement.direction == BULLET_MOVEMENT_DIRECTION_UP ? -1 : 1;
  bullet->position.z += bullet->movement.speed * direction;
}

void drawBullet(Bullet *bullet) {
  if (!bullet) {
    return;
  }
  updateBullet(bullet);
  DrawCylinder(
      (Vector3){bullet->position.x, bullet->position.y, bullet->position.z},
      0.1f, 0.2f, bullet->size.by_y, 10, RED);
}

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

void destroyBulletNode(BulletNode *node) {
  if (!node) {
    return;
  }
  free(node);
}

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
  return list;
}

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
}

void drawBullets(BulletList *list) {
  BulletNode *node = list->head;
  while (node) {
    drawBullet(&node->self);
    node = node->next;
  }
}

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