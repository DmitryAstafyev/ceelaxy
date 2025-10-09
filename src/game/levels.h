#ifndef LEVELS_H
#define LEVELS_H

#include "../models/models.h"
#include <stdint.h>

static const float INIT_LEVEL_UNIT_BULLET_ACCELERATION = 0.1f;
static const float INIT_LEVEL_UNIT_BULLET_INIT_SPEED = 1.0f;
static const float INIT_LEVEL_UNIT_BULLET_DELAY = 1.2f;
static const ModelId INIT_LEVEL_UNIT_MODEL = MODEL_CAMO_STELLAR_JET;

static const float INIT_LEVEL_PLAYER_BULLET_ACCELERATION = 0.1f;
static const float INIT_LEVEL_PLAYER_BULLET_INIT_SPEED = 1.0f;
static const float INIT_LEVEL_PLAYER_BULLET_DELAY = 0.2f;

typedef struct {
  float bullet_acceleration;
  float bullet_init_speed;
  float bullet_delay_spawn;
  uint16_t count;
  ModelId model;
  uint8_t max_col;
  uint8_t max_ln;
} LevelUnitsParameters;

typedef struct {
  float bullet_acceleration;
  float bullet_init_speed;
  float bullet_delay_spawn;
} LevelPlayerParameters;

typedef struct {
  uint16_t level;
  LevelUnitsParameters units;
  LevelPlayerParameters player;
} Level;

Level getFirstLevel();

#endif