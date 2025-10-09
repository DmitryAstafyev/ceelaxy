#include "levels.h"
#include <raylib.h>
#include <stdint.h>

Level getFirstLevel() {
  Level level;
  LevelUnitsParameters units;
  units.bullet_acceleration = INIT_LEVEL_UNIT_BULLET_ACCELERATION;
  units.bullet_init_speed = INIT_LEVEL_UNIT_BULLET_INIT_SPEED;
  units.bullet_delay_spawn = INIT_LEVEL_UNIT_BULLET_DELAY;
  units.damage_life = INIT_LEVEL_UNIT_DAMAGE_LIFE;
  units.damage_energy = INIT_LEVEL_UNIT_DAMAGE_ENERGY;
  units.count = 20;
  units.max_ln = 2;
  units.max_col = 10;
  units.model = INIT_LEVEL_UNIT_MODEL;
  LevelPlayerParameters player;
  player.bullet_acceleration = INIT_LEVEL_PLAYER_BULLET_ACCELERATION;
  player.bullet_init_speed = INIT_LEVEL_PLAYER_BULLET_INIT_SPEED;
  player.bullet_delay_spawn = INIT_LEVEL_PLAYER_BULLET_DELAY;
  player.damage_life = INIT_LEVEL_PLAYER_DAMAGE_LIFE;
  player.damage_energy = INIT_LEVEL_PLAYER_DAMAGE_ENERGY;
  level.level = 0;
  level.units = units;
  level.player = player;
  level.label_started_at = GetTime();
  return level;
}

Level goToNextLevel(Level level) {
  level.level += 1;
  level.label_started_at = GetTime();
  level.units.bullet_acceleration *= (1.0 + LEVEL_PARAMS_STEP);
  level.units.bullet_init_speed *= (1.0 + LEVEL_PARAMS_STEP);
  level.units.bullet_delay_spawn *= (1.0 - LEVEL_PARAMS_STEP);
  level.units.damage_life *= (1.0 + LEVEL_PARAMS_STEP);
  level.units.damage_energy *= (1.0 + LEVEL_PARAMS_STEP);

  level.player.bullet_acceleration *= (1.0 + LEVEL_PARAMS_STEP);
  level.player.bullet_init_speed *= (1.0 + LEVEL_PARAMS_STEP);
  level.player.bullet_delay_spawn *= (1.0 - LEVEL_PARAMS_STEP);
  level.player.damage_life *= (1.0 + LEVEL_PARAMS_STEP);
  level.player.damage_energy *= (1.0 + LEVEL_PARAMS_STEP);

  return level;
}

void levelDraw(Level *level) {
  if (!level)
    return;

  double elapsed = GetTime() - level->label_started_at;
  if (elapsed >= LEVEL_LABEL_DURATION)
    return; // надпись уже погасла

  float alpha = (float)(1.0 - (elapsed / LEVEL_LABEL_DURATION));
  if (alpha < 0.0f)
    alpha = 0.0f;
  if (alpha > 1.0f)
    alpha = 1.0f;

  const char *levelText = TextFormat("Level %d", level->level + 1);
  int font = LEVEL_LABEL_FONT_SIZE;

  int textW = MeasureText(levelText, font);
  int textH = font;
  int x = (GetScreenWidth() - textW) / 2;
  int y = (GetScreenHeight() - textH) / 2;

  DrawText(levelText, x + 2, y + 2, font, Fade(BLACK, alpha * 0.5f));
  DrawText(levelText, x, y, font, Fade(RAYWHITE, alpha));
}