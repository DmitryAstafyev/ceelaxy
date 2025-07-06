#ifndef BULLETS_H
#define BULLETS_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
  BULLET_MOVEMENT_DIRECTION_UP = 1,
  BULLET_MOVEMENT_DIRECTION_DOWN = 2
} BulletMovementDirection;

typedef struct {
  float x, y, z;
} BulletPosition;

typedef struct {
  float by_x, by_y, by_z;
} BulletSize;

typedef struct {
  float acceleration;
  float speed;
  float angle;
  uint8_t direction;
} BulletMovement;

typedef struct {
  BulletMovement movement;
  BulletPosition position;
  BulletSize size;
} Bullet;

typedef struct BulletNode {
  struct BulletNode *next;
  struct BulletNode *prev;
  Bullet self;
  size_t idx;
} BulletNode;

typedef struct {
  BulletNode *head;
  BulletNode *tail;
  uint16_t length;
  size_t idx;
  double last_spawn;
} BulletList;

BulletPosition newBulletPosition(float x, float y, float z);

BulletSize newBulletSize(float by_x, float by_y, float by_z);

Bullet newBullet(BulletMovementDirection direction, BulletPosition position,
                 BulletSize size);

BulletList *newBulletList();

void insertBulletIntoList(BulletList *list, Bullet bullet);

void drawBullets(BulletList *list);

void destroyBulletList(BulletList *list);
#endif