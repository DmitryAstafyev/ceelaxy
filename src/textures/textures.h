#pragma once

#include "raylib.h"
#include "rlgl.h"

static unsigned int TEX_ID_FIRE_SOFT = 0;
static unsigned int TEX_ID_FIRE_STREAK = 1;
static unsigned int TEX_ID_GLOW = 2;
static unsigned int TEX_ID_SMOKE_SOFT = 3;

typedef struct GameTexture {
  struct GameTexture *next;
  struct GameTexture *prev;
  Texture2D tex;
  int id;
} GameTexture;

typedef struct GameTextures {
  GameTexture *home;
  GameTexture *tail;
} GameTextures;

void destroyTexturesList(GameTextures *list);

GameTextures *createGameTexturesList(void);

GameTexture *getGameTextureById(GameTextures *list, unsigned int id);