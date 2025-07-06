#ifndef GAME_H
#define GAME_H

#include "../bullets/bullets.h"
#include "../models/models.h"
#include "../raylib/rlights.h"
#include "../units/player.h"
#include "../units/unit.h"
#include "raylib.h" // For Model, Texture2D, Shader
#include <stdbool.h>

typedef struct Game {
  UnitList *enemies;
  Player *player;
  BulletList *bullets;
  ShipModelList *models;
  Camera3D camera;
  Light light;
} Game;

Game *newGame(int height, int width);

bool renderGame(Game *game);

void destroyGame(Game *game);

void runGame(Game *game);

#endif