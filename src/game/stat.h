#ifndef GAME_STAT_H
#define GAME_STAT_H

static const unsigned int GAME_STAT_FONT_SIZE = 18;
static const unsigned int GAME_STAT_PADDING = 4;

static const unsigned int GAME_STAT_HIT_COST = 1;
static const unsigned int GAME_STAT_MISS_COST = 1;
static const unsigned int GAME_STAT_SHOOT_COST = 2;

typedef struct {
  int hits;
  int misses;
  int score;
} GameStat;

GameStat newGameStat();

void gameStatDraw(GameStat *stat);

void addHitIntoGameStat(GameStat *stat);

void addMissIntoGameStat(GameStat *stat);

void addShootIntoGameStat(GameStat *stat);

#endif
