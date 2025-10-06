#include "raylib.h"
#include "rlgl.h"
#include <raymath.h>
#include <stdbool.h>
#include <stddef.h>

#define TRAIL_MAX 128

typedef struct {
  Vector3 pos, vel;
  float size;
  float rot;
  float life, ttl;
  Color color;
} TrailParticle;

typedef struct {
  TrailParticle p[TRAIL_MAX];
  int count;
  Texture2D tex;
  bool additive;
  float spawnRate;
  float accum;
  float baseSize;
  float grow;
  float damping;
  float speed;
  Color baseColor;
} TrailEmitter;

TrailEmitter newTrailEmitter(Texture2D tex, bool additive);

void trailEmit(TrailEmitter *e, Vector3 origin, Vector3 dir, float dt);

void trailUpdate(TrailEmitter *e, float dt);

void trailDraw(TrailEmitter *e, Camera3D cam);