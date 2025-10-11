
#include "stat.h"
#include "raylib.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Initializes and returns a new GameStat structure with zeroed fields.
 *
 * @return A GameStat structure with hits, misses, and score set to zero.
 */
GameStat newGameStat()
{
  GameStat stat = {};
  stat.score = 0;
  stat.hits = 0;
  stat.misses = 0;
  return stat;
}

/**
 * @brief Renders the game statistics (hits, misses, score) on the screen.
 *
 * This function draws the statistics in the top-left corner of the screen,
 * using a predefined font size and padding for better visibility.
 *
 * @param stat Pointer to the GameStat structure containing the statistics to display.
 */
void gameStatDraw(GameStat *stat)
{

  const char *hitsText = TextFormat("Hits: %d", stat->hits);
  const char *missText = TextFormat("Misses: %d", stat->misses);
  const char *scoreText = TextFormat("Score: %d", stat->score);

  int w = MeasureText(hitsText, GAME_STAT_FONT_SIZE);
  int w2 = MeasureText(missText, GAME_STAT_FONT_SIZE);
  int w3 = MeasureText(scoreText, GAME_STAT_FONT_SIZE);
  int maxw = (w > w2) ? ((w > w3) ? w : w3) : ((w2 > w3) ? w2 : w3);
  int boxW = maxw + GAME_STAT_PADDING * 2;
  int boxH = GAME_STAT_FONT_SIZE * 2 + GAME_STAT_PADDING * 3;

  DrawRectangle(20 - GAME_STAT_PADDING, 20 - GAME_STAT_PADDING, boxW, boxH,
                Fade(BLACK, 0.5f));

  DrawText(hitsText, 20, 20, GAME_STAT_FONT_SIZE, RAYWHITE);
  DrawText(missText, 20, 20 + GAME_STAT_FONT_SIZE + GAME_STAT_PADDING,
           GAME_STAT_FONT_SIZE, RAYWHITE);
  DrawText(scoreText, 20, 20 + (GAME_STAT_FONT_SIZE + GAME_STAT_PADDING) * 2,
           GAME_STAT_FONT_SIZE, RAYWHITE);
}

/**
 * @brief Increments the hit count and updates the score accordingly.
 *
 * Each hit increases the score by a predefined amount.
 *
 * @param stat Pointer to the GameStat structure to update.
 */
void addHitIntoGameStat(GameStat *stat)
{
  stat->hits += 1;
  stat->score += GAME_STAT_HIT_COST;
}

/**
 * @brief Increments the miss count and updates the score accordingly.
 *
 * Each miss decreases the score by a predefined amount.
 *
 * @param stat Pointer to the GameStat structure to update.
 */
void addMissIntoGameStat(GameStat *stat)
{
  stat->misses += 1;
  stat->score -= GAME_STAT_MISS_COST;
}

/**
 * @brief Increments the shoot count and updates the score accordingly.
 *
 * Each shoot decreases the score by a predefined amount.
 *
 * @param stat Pointer to the GameStat structure to update.
 */
void addShootIntoGameStat(GameStat *stat)
{
  stat->score -= GAME_STAT_SHOOT_COST;
}