#pragma once

#include "raylib.h"
#include "rlgl.h"

typedef enum GAME_TEXTURES {
  TEX_ID_FIRE_SOFT = 0,
  TEX_ID_FIRE_STREAK = 1,
  TEX_ID_GLOW = 2,
  TEX_ID_SMOKE_SOFT = 3,
} GAME_TEXTURES;

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

GameTexture *getGameTextureById(GameTextures *list, int id);