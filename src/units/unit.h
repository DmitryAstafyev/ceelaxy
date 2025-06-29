#ifndef UNIT_H
#define UNIT_H

#include "../movement/movement.h"
#include <SDL.h>
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
  uint16_t x;
  uint16_t y;
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
} Unit;

UnitSize UnitSize_new();

UnitPosition UnitPosition_new();

UnitRender UnitRender_new(UnitPosition position);

UnitState UnitState_new();

Unit Unit_new(UnitType ty);

typedef struct UnitNode {
  struct UnitNode *prev;
  struct UnitNode *next;
  Unit self;
} UnitNode;

typedef struct {
  UnitNode *head;
  UnitNode *tail;
  size_t length;
} UnitList;

UnitNode *UnitNode_new(UnitNode *prev, Unit unit, int window_width);

void Unit_render(SDL_Renderer *renderer, Unit *unit);

UnitList *UnitList_new(int count, int width);

void UnitList_destroy(UnitList *list);

void UnitList_insert(UnitList *list, Unit unit, int window_width);

void UnitList_render(SDL_Renderer *renderer, UnitList *list);

#endif