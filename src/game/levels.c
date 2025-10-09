#include "levels.h"
#include <stdint.h>

Level getFirstLevel() {
  Level level;
  LevelUnitsParameters units;
  units.bullet_acceleration = INIT_LEVEL_UNIT_BULLET_ACCELERATION;
  units.bullet_init_speed = INIT_LEVEL_UNIT_BULLET_INIT_SPEED;
  units.bullet_delay_spawn = INIT_LEVEL_UNIT_BULLET_DELAY;
  units.count = 20;
  units.max_ln = 2;
  units.max_col = 10;
  units.model = INIT_LEVEL_UNIT_MODEL;
  LevelPlayerParameters player;
  player.bullet_acceleration = INIT_LEVEL_PLAYER_BULLET_ACCELERATION;
  player.bullet_init_speed = INIT_LEVEL_PLAYER_BULLET_INIT_SPEED;
  player.bullet_delay_spawn = INIT_LEVEL_PLAYER_BULLET_DELAY;
  level.level = 0;
  level.units = units;
  level.player = player;
  return level;
}