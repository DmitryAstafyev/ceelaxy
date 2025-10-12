// trail.c
// Implements a simple particle trail emitter for visual effects.
// ================================================
// AI usage note: This module was developed based on AI-generated (ChatGPT) code.
// The author made additional changes and performed partial refactoring.
// ================================================

#include "trail.h"
#include "raylib.h"
#include "rlgl.h"
#include <raymath.h>
#include <stdbool.h>
#include <stddef.h>

// Simple random float in [a, b]
static inline float frand(float a, float b)
{
  return a + (b - a) * ((float)GetRandomValue(0, 10000) / 10000.0f);
}

/**
 * @brief Creates and initializes a new TrailEmitter instance.
 *
 * This function sets up a TrailEmitter with default parameters for
 * spawning, size, growth, damping, speed, and color. The provided texture
 * and blending mode are applied to the emitter.
 *
 * @param tex The texture to use for trail particles.
 * @param additive If true, uses additive blending; otherwise, uses alpha blending.
 * @return A fully initialized TrailEmitter object.
 */
TrailEmitter newTrailEmitter(Texture2D tex, bool additive)
{
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

/**
 * @brief Emits new trail particles from the emitter at the specified origin and direction.
 *
 * This function spawns new particles based on the emitter's spawn rate and
 * the elapsed time. Each particle is initialized with random jitter, size,
 * rotation, lifespan, and color.
 *
 * @param emitter Pointer to the TrailEmitter instance.
 * @param origin The 3D position from which to emit particles.
 * @param dir The direction vector for particle emission.
 * @param dt Time elapsed since the last update (in seconds).
 */
void trailEmit(TrailEmitter *emitter, Vector3 origin, Vector3 dir, float dt)
{
  emitter->accum += emitter->spawnRate * dt;
  while (emitter->accum >= 1.0f && emitter->count < TRAIL_MAX)
  {

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

/**
 * @brief Updates the state of all active trail particles in the emitter.
 *
 * This function moves each particle according to its velocity, applies
 * damping, increases its size, and decreases its lifespan. Particles that
 * have expired (life <= 0) are removed from the emitter.
 *
 * @param e Pointer to the TrailEmitter instance.
 * @param dt Time elapsed since the last update (in seconds).
 */
void trailUpdate(TrailEmitter *e, float dt)
{
  int w = 0;
  for (int r = 0; r < e->count; ++r)
  {
    TrailParticle *q = &e->p[r];
    q->pos = Vector3Add(q->pos, Vector3Scale(q->vel, dt));
    q->vel = Vector3Scale(q->vel, e->damping);
    q->size += e->grow * dt;
    q->life -= dt;
    if (q->life > 0)
    {
      float t = q->life / q->ttl;
      q->color.a = (unsigned char)(220 * t);
      e->p[w++] = *q;
    }
  }
  e->count = w;
}

/**
 * @brief Renders all active trail particles using the specified camera.
 *
 * This function draws each particle as a billboarded quad using the emitter's
 * texture. The blending mode is set based on the emitter's configuration.
 *
 * @param e Pointer to the TrailEmitter instance.
 * @param cam The Camera3D used for rendering the scene.
 */
void trailDraw(TrailEmitter *e, Camera3D cam)
{
  if (e->count == 0)
    return;
  BeginBlendMode(e->additive ? BLEND_ADDITIVE : BLEND_ALPHA);

  Rectangle src = {0, 0, (float)e->tex.width, (float)e->tex.height};
  Vector3 up = {0, 1, 0};

  for (int i = 0; i < e->count; ++i)
  {
    TrailParticle *q = &e->p[i];
    Vector2 size = {q->size, q->size};
    Vector2 origin = {q->size * 0.5f, q->size * 0.5f};
    DrawBillboardPro(cam, e->tex, src, q->pos, up, size, origin, q->rot,
                     q->color);
  }

  EndBlendMode();
}
