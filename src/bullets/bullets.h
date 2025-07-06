#ifndef BULLETS_H
#define BULLETS_H

#include "../units/unit.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static const double BULLET_HIT_SEN_TIME = 0.1f;

typedef enum {
  BULLET_MOVEMENT_DIRECTION_UP = 1,
  BULLET_MOVEMENT_DIRECTION_DOWN = 2
} BulletMovementDirection;

typedef struct {
  uint8_t health;
  uint8_t energy;
} BulletParameters;

typedef struct {
  float x, y, z;
} BulletPosition;

typedef struct {
  float by_x, by_y, by_z;
  float radius_top, radius_bottom;
  float slices;
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
  BulletParameters params;
  bool alive;
} Bullet;

typedef struct BulletNode {
  struct BulletNode *next;
  struct BulletNode *prev;
  Bullet self;
  size_t idx;
} BulletNode;

typedef struct BulletAreaFrame {
  float top;
  float bottom;
} BulletAreaFrame;

typedef struct {
  BulletNode *head;
  BulletNode *tail;
  uint16_t length;
  size_t idx;
  double last_spawn;
  BulletAreaFrame frame;
} BulletList;

BulletPosition newBulletPosition(float x, float y, float z);

BulletSize newBulletSize(float by_x, float by_y, float by_z);

BulletParameters newBulletParameters(uint8_t health, uint8_t energy);

Bullet newBullet(BulletMovementDirection direction, BulletPosition position,
                 BulletSize size, BulletParameters params);

BulletList *newBulletList();

void insertBulletIntoList(BulletList *list, Bullet bullet);

void drawBullets(BulletList *list);

void checkBulletHitsUnits(UnitList *units, BulletList *bullets);

void destroyBulletList(BulletList *list);
#endif