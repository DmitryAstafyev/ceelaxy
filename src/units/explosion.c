// explosion.c

#include "explosion.h"
#include "raymath.h"
#include "rlgl.h"
#include <math.h>

static inline float frand(float a, float b) {
  return a + (b - a) * (float)GetRandomValue(0, 10000) / 10000.0f;
}

static inline Vector3 randInSphere(float vmin, float vmax) {
  float th = frand(0, 2 * PI);
  float ct = frand(-1, 1);
  float st = sqrtf(1 - ct * ct);
  float r = frand(vmin, vmax);
  return (Vector3){r * st * cosf(th), r * ct, r * st * sinf(th)};
}

BulletExplosion newBulletExplosion(Texture2D fire, Texture2D smoke,
                                   Texture2D glow) {
  BulletExplosion e = {0};
  e.active = false;
  e.count = 0;

  e.gravityY = -5.5f;
  e.damping = 1.05f;
  e.backDrift = 4.0f;

  // how much particles follow moving ship (per-frame origin delta)
  e.carrySmoke = 0.60f;
  e.carryFire = 0.20f;
  e.carrySpark = 0.10f;

  e.texFire = fire;
  e.texSmoke = smoke;
  e.texGlow = glow;
  return e;
}

void bulletExplosionSpawnAt(BulletExplosion *e, Vector3 origin,
                            const Camera3D *cam) {
  e->active = true;
  e->spawn_origin = origin;
  e->last_origin = origin;

  Vector3 forward =
      Vector3Normalize(Vector3Subtract(cam->target, cam->position));

  // FIRE
  int nFire = 100;
  for (int i = 0; i < nFire && e->count < EXP_MAX; ++i) {
    ExpParticle *q = &e->p[e->count++];
    q->kind = EXP_FIRE;
    q->pos = origin;
    q->vel = Vector3Add(randInSphere(6.0f, 11.0f), Vector3Scale(forward, 2.0f));
    q->size = frand(0.25f, 0.9f);
    q->rot = frand(0, 360);
    q->ttl = frand(0.25f, 0.45f);
    q->life = q->ttl;
    q->color = (Color){255, 230, 140, 255};
  }

  // SMOKE
  int nSmoke = 80;
  for (int i = 0; i < nSmoke && e->count < EXP_MAX; ++i) {
    ExpParticle *q = &e->p[e->count++];
    q->kind = EXP_SMOKE;
    q->pos = origin;
    q->vel = Vector3Add(randInSphere(1.5f, 4.0f), Vector3Scale(forward, 1.5f));
    q->size = frand(0.35f, 1.0f);
    q->rot = frand(0, 360);
    q->ttl = frand(0.9f, 1.6f);
    q->life = q->ttl;
    q->color = (Color){180, 180, 180, 220};
  }

  // SPARKS
  int nSpark = 60;
  for (int i = 0; i < nSpark && e->count < EXP_MAX; ++i) {
    ExpParticle *q = &e->p[e->count++];
    q->kind = EXP_SPARK;
    q->pos = origin;
    q->vel =
        Vector3Add(randInSphere(10.0f, 16.0f), Vector3Scale(forward, 2.5f));
    q->size = frand(0.08f, 0.7f);
    q->rot = frand(0, 360);
    q->ttl = frand(0.35f, 0.7f);
    q->life = q->ttl;
    q->color = (Color){255, 200, 60, 255};
  }
}

void bulletExplosionUpdate(BulletExplosion *e, Vector3 origin, float dt,
                           const Camera3D *cam) {
  if (!e || !e->active)
    return;

  Vector3 forward =
      Vector3Normalize(Vector3Subtract(cam->target, cam->position));
  Vector3 drift = Vector3Scale(forward, e->backDrift);

  // per-frame ship movement delta
  Vector3 originDelta = Vector3Subtract(origin, e->last_origin);
  e->last_origin = origin;

  int w = 0;
  for (int r = 0; r < e->count; ++r) {
    ExpParticle *q = &e->p[r];

    // gravity + damping + scene back drift
    q->vel.y += e->gravityY * dt;
    q->vel = Vector3Scale(q->vel, e->damping);
    q->vel = Vector3Add(q->vel, Vector3Scale(drift, dt));

    // carry by moving ship (smoke follows more than fire/sparks)
    float carry = (q->kind == EXP_SMOKE)  ? e->carrySmoke
                  : (q->kind == EXP_FIRE) ? e->carryFire
                                          : e->carrySpark;
    q->pos = Vector3Add(q->pos, Vector3Scale(originDelta, carry));

    // integrate position and size
    q->pos = Vector3Add(q->pos, Vector3Scale(q->vel, dt));
    if (q->kind == EXP_SMOKE)
      q->size += 0.35f * dt;
    if (q->kind == EXP_FIRE)
      q->size -= 0.15f * dt;

    // lifetime / color
    q->life -= dt;
    if (q->life > 0) {
      float t = 1.0f - (q->life / q->ttl); // 0..1
      if (q->kind == EXP_FIRE || q->kind == EXP_SPARK) {
        float hue = 50.0f + (5.0f - 50.0f) * t;
        Color c = ColorFromHSV(hue, 0.95f, 1.0f);
        unsigned char a = (unsigned char)(255.0f * powf(1.0f - t, 0.4f));
        q->color = (Color){c.r, c.g, c.b, a};
      } else {
        unsigned char a = (unsigned char)(200.0f * (1.0f - t));
        int g = (int)(160 + 20 * t);
        q->color =
            (Color){(unsigned char)g, (unsigned char)g, (unsigned char)g, a};
      }
      e->p[w++] = *q;
    }
  }
  e->count = w;
  if (e->count == 0)
    e->active = false;
}

void bulletExplosionDraw(BulletExplosion *e, Camera3D cam) {
  if (!e || e->count == 0)
    return;

  Rectangle sFire = {0, 0, (float)e->texFire.width, (float)e->texFire.height};
  Rectangle sSmoke = {0, 0, (float)e->texSmoke.width,
                      (float)e->texSmoke.height};
  Rectangle sGlow = {0, 0, (float)e->texGlow.width, (float)e->texGlow.height};
  Vector3 up = (Vector3){0, 1, 0};

  // smoke first (alpha)
  BeginBlendMode(BLEND_ALPHA);
  for (int i = 0; i < e->count; ++i)
    if (e->p[i].kind == EXP_SMOKE) {
      ExpParticle *q = &e->p[i];
      Vector2 size = (Vector2){q->size, q->size};
      Vector2 org = (Vector2){q->size * 0.5f, q->size * 0.5f};
      DrawBillboardPro(cam, e->texSmoke, sSmoke, q->pos, up, size, org, q->rot,
                       q->color);
    }
  EndBlendMode();

  // fire/sparks + halo (additive)
  BeginBlendMode(BLEND_ADDITIVE);
  for (int i = 0; i < e->count; ++i)
    if (e->p[i].kind != EXP_SMOKE) {
      ExpParticle *q = &e->p[i];
      Vector2 size = (Vector2){q->size, q->size};
      Vector2 org = (Vector2){q->size * 0.5f, q->size * 0.5f};
      DrawBillboardPro(cam, e->texFire, sFire, q->pos, up, size, org, q->rot,
                       q->color);
      if (e->texGlow.id) {
        float gs = q->size * 1.6f;
        Vector2 gsz = (Vector2){gs, gs};
        Vector2 gor = (Vector2){gs * 0.5f, gs * 0.5f};
        Color gcol =
            (Color){255, 255, 255, (unsigned char)(q->color.a * 0.35f)};
        DrawBillboardPro(cam, e->texGlow, sGlow, q->pos, up, gsz, gor, 0.0f,
                         gcol);
      }
    }
  EndBlendMode();
}
