
#include "stat.h"
#include "raylib.h"
#include <stdbool.h>
#include <stdint.h>

GameStat newGameStat() {
  GameStat stat = {};
  stat.score = 0;
  stat.hits = 0;
  stat.misses = 0;
  return stat;
}

void gameStatDraw(GameStat *stat) {

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

void addHitIntoGameStat(GameStat *stat) {
  stat->hits += 1;
  stat->score += GAME_STAT_HIT_COST;
}

void addMissIntoGameStat(GameStat *stat) {
  stat->misses += 1;
  stat->score -= GAME_STAT_MISS_COST;
}

void addShootIntoGameStat(GameStat *stat) {
  stat->score -= GAME_STAT_SHOOT_COST;
}