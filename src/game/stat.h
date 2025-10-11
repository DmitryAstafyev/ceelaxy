#ifndef GAME_STAT_H
#define GAME_STAT_H

// Game statistics display parameters
static const unsigned int GAME_STAT_FONT_SIZE = 18;
// Padding around the statistics text
static const unsigned int GAME_STAT_PADDING = 4;

// Score impact values
static const unsigned int GAME_STAT_HIT_COST = 1;
// Miss impact values
static const unsigned int GAME_STAT_MISS_COST = 1;
// Shoot impact values
static const unsigned int GAME_STAT_SHOOT_COST = 2;

/**
 * @brief Structure to track game statistics such as hits, misses, and score.
 */
typedef struct
{
  int hits;   /// Number of successful hits
  int misses; /// Number of missed shots
  int score;  /// Current score based on hits and misses
} GameStat;

/**
 * @brief Initializes and returns a new GameStat structure with zeroed fields.
 *
 * @return A GameStat structure with hits, misses, and score set to zero.
 */
GameStat newGameStat();

/**
 * @brief Renders the game statistics (hits, misses, score) on the screen.
 *
 * This function draws the statistics in the top-left corner of the screen,
 * using a predefined font size and padding for better visibility.
 *
 * @param stat Pointer to the GameStat structure containing the statistics to display.
 */
void gameStatDraw(GameStat *stat);

/**
 * @brief Increments the hit count and updates the score accordingly.
 *
 * Each hit increases the score by a predefined amount.
 *
 * @param stat Pointer to the GameStat structure to update.
 */
void addHitIntoGameStat(GameStat *stat);

/**
 * @brief Increments the miss count and updates the score accordingly.
 *
 * Each miss decreases the score by a predefined amount.
 *
 * @param stat Pointer to the GameStat structure to update.
 */
void addMissIntoGameStat(GameStat *stat);

/**
 * @brief Increments the shoot count and updates the score accordingly.
 *
 * Each shoot decreases the score by a predefined amount.
 *
 * @param stat Pointer to the GameStat structure to update.
 */
void addShootIntoGameStat(GameStat *stat);

#endif
