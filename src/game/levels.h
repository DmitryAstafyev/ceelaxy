#ifndef LEVELS_H
#define LEVELS_H

#include "../models/models.h"
#include <stdint.h>

#define LEVEL_LABEL_DURATION 5.0

// Initial level parameters for enemy units
#define INIT_LEVEL_UNIT_BULLET_ACCELERATION 0.005f
// Initial speed of enemy bullets
#define INIT_LEVEL_UNIT_BULLET_INIT_SPEED 0.8f
// Delay between enemy bullet spawns
#define INIT_LEVEL_UNIT_BULLET_DELAY 1.2f
// Number of enemy units in the level
#define INIT_LEVEL_UNIT_DAMAGE_LIFE 5.0f
// Damage dealt by enemy units
#define INIT_LEVEL_UNIT_DAMAGE_ENERGY 10.0f
// Energy cost or impact of enemy units
static const ModelId INIT_LEVEL_UNIT_MODEL = MODEL_INTERSTELLAR_RUNNER;

// Initial level parameters for the player
#define INIT_LEVEL_PLAYER_BULLET_ACCELERATION 0.01f
// Initial acceleration of player bullets
#define INIT_LEVEL_PLAYER_BULLET_INIT_SPEED 2.0f
// Initial speed of player bullets
#define INIT_LEVEL_PLAYER_BULLET_DELAY 0.2f
// Delay between player bullet spawns
#define INIT_LEVEL_PLAYER_DAMAGE_LIFE 20.0f
// Damage dealt by player bullets
#define INIT_LEVEL_PLAYER_DAMAGE_ENERGY 10.0f

// Incremental step for level parameter adjustments
#define LEVEL_PARAMS_STEP 0.05f
// Total number of levels defined
#define LEVELS_COUNT 10

// Font size for level labels
#define LEVEL_LABEL_FONT_SIZE 32

/**
 * @brief Parameters defining enemy units for a specific level.
 */
typedef struct
{
  float bullet_acceleration; /// Acceleration of enemy bullets
  float bullet_init_speed;   /// Initial speed of enemy bullets
  float bullet_delay_spawn;  /// Delay between enemy bullet spawns
  float damage_life;         /// Damage dealt by enemy units
  float damage_energy;       /// Energy cost or impact of enemy units
  uint16_t count;            /// Number of enemy units in the level
  ModelId model;             /// 3D model ID for enemy units
  uint8_t max_col;           /// Maximum columns in enemy formation
  uint8_t max_ln;            /// Maximum lines in enemy formation
} LevelUnitsParameters;

/**
 * @brief Parameters defining player attributes for a specific level.
 */
typedef struct
{
  float bullet_acceleration; /// Acceleration of player bullets
  float bullet_init_speed;   /// Initial speed of player bullets
  float bullet_delay_spawn;  /// Delay between player bullet spawns
  float damage_life;         /// Damage dealt by player bullets
  float damage_energy;       /// Energy cost or impact of player bullets
} LevelPlayerParameters;

/**
 * @brief Represents a game level with its parameters and state.
 */
typedef struct
{
  uint16_t level;               /// Current level number
  double label_started_at;      /// Time when the level label started displaying
  LevelUnitsParameters units;   /// Enemy unit parameters for the level
  LevelPlayerParameters player; /// Player parameters for the level
} Level;

/**
 * @brief Retrieves the parameters for the first level of the game.
 *
 * @return A Level struct initialized with the first level's parameters.
 */
Level getFirstLevel();

/**
 * @brief Advances to the next level, incrementing difficulty parameters.
 *
 * Each parameter is increased by a fixed step, and the level number is
 * incremented. If the current level is the last defined level, it wraps
 * around to the first level.
 *
 * @param level The current Level struct.
 * @return A new Level struct with updated parameters for the next level.
 */
Level goToNextLevel(Level level);

/**
 * @brief Renders the level label on the screen if within the display duration.
 *
 * This function checks if the level label should still be displayed based
 * on the elapsed time since it started. If so, it draws the label centered
 * on the screen with a shadow effect for better visibility.
 *
 * @param level Pointer to the Level struct containing label timing information.
 */
void levelDraw(Level *level);

#endif