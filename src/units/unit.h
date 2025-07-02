#ifndef UNIT_H
#define UNIT_H

#include "../models/models.h"
#include "../movement/movement.h"
#include "raylib.h" // For Model, Texture2D, Shader
#include <stddef.h>
#include <stdint.h>

typedef enum UnitType {
  UNIT_TYPE_SOLDER = 1,
  UNIT_TYPE_ENEMY = 2,
} UnitType;

typedef struct UnitState {
  uint8_t health;
  uint8_t energy;
} UnitState;

typedef struct UnitPosition {
  float x;
  float y;
  float z;
  uint16_t ln;
  uint16_t col;
} UnitPosition;

typedef struct UnitSize {
  uint16_t height;
  uint16_t width;
} UnitSize;

typedef struct UnitRender {
  UnitPosition position;
  UnitSize size;
  MovementAction *action;
  uint32_t last_frame;
} UnitRender;

typedef struct Unit {
  UnitType type;
  UnitState state;
  UnitRender render;
  ShipModel *model;
} Unit;

UnitSize newUnitSize();

UnitPosition newUnitPosition();

UnitRender newUnitRender(UnitPosition position);

UnitState newUnitState();

Unit newUnit(UnitType ty, ShipModel *model);

typedef struct UnitNode {
  struct UnitNode *prev;
  struct UnitNode *next;
  Unit self;
} UnitNode;

typedef struct {
  UnitNode *head;
  UnitNode *tail;
  uint16_t length;
} UnitList;

UnitNode *newUnitNode(UnitNode *prev, Unit unit, int window_width);

Matrix getMatrixUnit(Unit *unit);

UnitList *newUnitList(int count, int width, ShipModel *model);

void destroyUnitList(UnitList *list);

void insertToUnitList(UnitList *list, Unit unit, int window_width);

Matrix *getMatrixFromUnitList(UnitList *list);

#endif