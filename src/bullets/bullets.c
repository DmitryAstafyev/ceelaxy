#include "bullets.h"
#include "raylib.h" // For Model, Texture2D, Shader
#include <raymath.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

BulletAreaFrame newBulletAreaFrame() {
  BulletAreaFrame frame;
  frame.top = -60.0f;
  frame.bottom = 60.0f;
  return frame;
}

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
  size.radius_top = 0.1f;
  size.radius_bottom = 0.5f;
  size.slices = 5;
  return size;
}

Bullet newBullet(BulletMovementDirection direction, BulletPosition position,
                 BulletSize size) {
  Bullet bullet;
  bullet.movement = newBulletMovement(direction);
  bullet.position = position;
  bullet.size = size;
  bullet.alive = true;
  return bullet;
}

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
  list->frame = newBulletAreaFrame();
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
  list->length += 1;
  printf("[Bullets] bullet has been spawn: %f, %f, %f\n", bullet.position.x,
         bullet.position.y, bullet.position.z);
}

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