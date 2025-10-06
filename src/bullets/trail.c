#include "trail.h"
#include "raylib.h"
#include "rlgl.h"
#include <raymath.h>
#include <stdbool.h>
#include <stddef.h>

static inline float frand(float a, float b) {
  return a + (b - a) * ((float)GetRandomValue(0, 10000) / 10000.0f);
}

TrailEmitter newTrailEmitter(Texture2D tex, bool additive) {
  TrailEmitter emitter = {0};
  emitter.tex = tex;
  emitter.additive = additive;
  emitter.spawnRate = 60.0f;
  emitter.baseSize = 1.5f;
  emitter.grow = 1.5f;
  emitter.damping = 0.92f;
  emitter.speed = 2.2f;
  emitter.baseColor = (Color){255, 230, 120, 255};
  return emitter;
}

void trailEmit(TrailEmitter *emitter, Vector3 origin, Vector3 dir, float dt) {
  emitter->accum += emitter->spawnRate * dt;
  while (emitter->accum >= 1.0f && emitter->count < TRAIL_MAX) {

    TrailParticle *q = &emitter->p[emitter->count++];

    Vector3 jitter = {frand(-0.25f, 0.25f), frand(-0.25f, 0.25f),
                      frand(-0.25f, 0.25f)};
    Vector3 v = Vector3Add(Vector3Scale(dir, -emitter->speed), jitter);

    q->pos = origin;
    q->vel = v;
    q->size = emitter->baseSize * frand(0.9f, 1.2f);
    q->rot = frand(0.0f, 360.0f);
    q->ttl = frand(0.35f, 0.55f);
    q->life = q->ttl;
    q->color = emitter->baseColor;

    emitter->accum -= 1.0f;
  }
}

void trailUpdate(TrailEmitter *e, float dt) {
  int w = 0;
  for (int r = 0; r < e->count; ++r) {
    TrailParticle *q = &e->p[r];
    q->pos = Vector3Add(q->pos, Vector3Scale(q->vel, dt));
    q->vel = Vector3Scale(q->vel, e->damping);
    q->size += e->grow * dt;
    q->life -= dt;
    if (q->life > 0) {
      float t = q->life / q->ttl;
      q->color.a = (unsigned char)(220 * t);
      e->p[w++] = *q;
    }
  }
  e->count = w;
}

void trailDraw(TrailEmitter *e, Camera3D cam) {
  if (e->count == 0)
    return;
  BeginBlendMode(e->additive ? BLEND_ADDITIVE : BLEND_ALPHA);

  Rectangle src = {0, 0, (float)e->tex.width, (float)e->tex.height};
  Vector3 up = {0, 1, 0};

  for (int i = 0; i < e->count; ++i) {
    TrailParticle *q = &e->p[i];
    Vector2 size = {q->size, q->size};
    Vector2 origin = {q->size * 0.5f, q->size * 0.5f};
    DrawBillboardPro(cam, e->tex, src, q->pos, up, size, origin, q->rot,
                     q->color);
  }

  EndBlendMode();
}
