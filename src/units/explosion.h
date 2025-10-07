// explosion.h

#pragma once
#include "raylib.h"

#define EXP_MAX 256

typedef enum { EXP_FIRE = 0, EXP_SMOKE = 1, EXP_SPARK = 2 } ExpKind;

typedef struct {
  Vector3 pos, vel;
  float size, rot;
  float life, ttl;
  ExpKind kind;
  Color color;
} ExpParticle;

typedef struct {
  ExpParticle p[EXP_MAX];
  int count;
  bool active; // false until the first update-spawn

  Vector3 spawn_origin; // origin at spawn moment
  Vector3 last_origin;  // origin from previous frame (to compute delta)

  // physics
  float gravityY;   // downward sink
  float damping;    // velocity damping
  float backDrift;  // drift along camera forward (m/s)
  float carrySmoke; // how much smoke follows moving ship [0..1]
  float carryFire;  // how much fire follows
  float carrySpark; // how much sparks follow

  // textures
  Texture2D texFire;
  Texture2D texSmoke;
  Texture2D texGlow; // optional halo
} BulletExplosion;

BulletExplosion newBulletExplosion(Texture2D fire, Texture2D smoke,
                                   Texture2D glow);

void bulletExplosionSpawnAt(BulletExplosion *e, Vector3 origin,
                            const Camera3D *cam);

void bulletExplosionUpdate(BulletExplosion *e, Vector3 origin, float dt,
                           const Camera3D *cam);

// Draw inside BeginMode3D(...).
void bulletExplosionDraw(BulletExplosion *e, Camera3D cam);

// Convenience check.
static inline bool bulletExplosionIsDead(const BulletExplosion *e) {
  return !e || e->count == 0;
}
