#ifndef LEVELS_H
#define LEVELS_H

#include "../models/models.h"
#include <stdint.h>

#define LEVEL_LABEL_DURATION 5.0

static const float INIT_LEVEL_UNIT_BULLET_ACCELERATION = 0.005f;
static const float INIT_LEVEL_UNIT_BULLET_INIT_SPEED = 0.8f;
static const float INIT_LEVEL_UNIT_BULLET_DELAY = 1.2f;
static const float INIT_LEVEL_UNIT_DAMAGE_LIFE = 5.0f;
static const float INIT_LEVEL_UNIT_DAMAGE_ENERGY = 10.0f;
static const ModelId INIT_LEVEL_UNIT_MODEL = MODEL_INTERSTELLAR_RUNNER;

static const float INIT_LEVEL_PLAYER_BULLET_ACCELERATION = 0.01f;
static const float INIT_LEVEL_PLAYER_BULLET_INIT_SPEED = 2.0f;
static const float INIT_LEVEL_PLAYER_BULLET_DELAY = 0.2f;
static const float INIT_LEVEL_PLAYER_DAMAGE_LIFE = 20.0f;
static const float INIT_LEVEL_PLAYER_DAMAGE_ENERGY = 10.0f;

static const float LEVEL_PARAMS_STEP = 0.05f;
static const unsigned int LEVELS_COUNT = 10;

static int LEVEL_LABEL_FONT_SIZE = 32;

typedef struct {
  float bullet_acceleration;
  float bullet_init_speed;
  float bullet_delay_spawn;
  float damage_life;
  float damage_energy;
  uint16_t count;
  ModelId model;
  uint8_t max_col;
  uint8_t max_ln;
} LevelUnitsParameters;

typedef struct {
  float bullet_acceleration;
  float bullet_init_speed;
  float bullet_delay_spawn;
  float damage_life;
  float damage_energy;
} LevelPlayerParameters;

typedef struct {
  uint16_t level;
  double label_started_at;
  LevelUnitsParameters units;
  LevelPlayerParameters player;
} Level;

Level getFirstLevel();

Level goToNextLevel(Level level);

void levelDraw(Level *level);

#endif