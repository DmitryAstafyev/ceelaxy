#include "levels.h"
#include "../utils/debug.h"
#include <raylib.h>
#include <stdint.h>

/**
 * @brief Initializes and returns the parameters for the first game level.
 *
 * Sets up enemy unit parameters and player parameters with initial values.
 * The enemy unit model is set to a predefined constant. The level number is
 * initialized to 0, and the label start time is set to the current time.
 *
 * @return A Level structure containing the initial level parameters.
 */
Level getFirstLevel()
{
  Level level;
  LevelUnitsParameters units;
  units.bullet_acceleration = INIT_LEVEL_UNIT_BULLET_ACCELERATION;
  units.bullet_init_speed = INIT_LEVEL_UNIT_BULLET_INIT_SPEED;
  units.bullet_delay_spawn = INIT_LEVEL_UNIT_BULLET_DELAY;
  units.damage_life = is_debug_mode ? 0.001f : INIT_LEVEL_UNIT_DAMAGE_LIFE;
  units.damage_energy = is_debug_mode ? 0.001f : INIT_LEVEL_UNIT_DAMAGE_ENERGY;
  units.count = 20;
  units.max_ln = 2;
  units.max_col = 10;
  units.model = INIT_LEVEL_UNIT_MODEL;
  LevelPlayerParameters player;
  player.bullet_acceleration = INIT_LEVEL_PLAYER_BULLET_ACCELERATION;
  player.bullet_init_speed = INIT_LEVEL_PLAYER_BULLET_INIT_SPEED;
  player.bullet_delay_spawn = INIT_LEVEL_PLAYER_BULLET_DELAY;
  player.damage_life =
      (is_debug_mode ? 100.0 : 1.0) * INIT_LEVEL_PLAYER_DAMAGE_LIFE;
  player.damage_energy =
      (is_debug_mode ? 100.0 : 1.0) * INIT_LEVEL_PLAYER_DAMAGE_ENERGY;
  level.level = 0;
  level.units = units;
  level.player = player;
  level.label_started_at = GetTime();
  return level;
}

/**
 * @brief Advances the game to the next level, updating enemy and player parameters.
 *
 * This function increments the level number and adjusts various parameters for
 * both enemy units and the player by a fixed step percentage. The label start
 * time is updated to the current time. The adjustments make the game progressively
 * more challenging.
 *
 * @param level The current Level structure to be updated.
 * @return The updated Level structure with incremented level number and adjusted parameters.
 */
Level goToNextLevel(Level level)
{
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

/**
 * @brief Renders the level label on the screen with a fade-out effect.
 *
 * This function displays the current level number at the center of the screen.
 * The label fades out over a predefined duration. If the elapsed time since
 * the label started displaying exceeds the duration, the function returns
 * without rendering anything.
 *
 * @param level Pointer to the Level structure containing the level number and label start time.
 */
void levelDraw(Level *level)
{
  if (!level)
    return;

  double elapsed = GetTime() - level->label_started_at;
  if (elapsed >= LEVEL_LABEL_DURATION)
    return;

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