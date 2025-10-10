// parallax.c
#include "parallax.h"
#include "../units/player.h" // adjust path if needed

#include <limits.h>
#include <math.h>
#include <stdlib.h>

// -------------------- helpers --------------------

static inline float clamp01(float x) { return x < 0 ? 0 : (x > 1 ? 1 : x); }

static inline float smoothstep(float e0, float e1, float x) {
  if (e0 == e1)
    return x >= e1 ? 1.0f : 0.0f;
  x = clamp01((x - e0) / (e1 - e0));
  return x * x * (3.0f - 2.0f * x);
}

static inline float frand01(unsigned int *state) {
  *state = (*state * 1664525u + 1013904223u);
  return (float)(*state) / (float)UINT_MAX;
}

static Texture2D makeDotTexture(void) {
  // 1x1 white tex; swap to a round sprite if you have one.
  Image img = GenImageColor(1, 1, WHITE);
  Texture2D t = LoadTextureFromImage(img);
  UnloadImage(img);
  return t;
}

static Color colorWithAlpha(Color c, float a) {
  c.a = (unsigned char)Clamp(a * 255.0f, 0.0f, 255.0f);
  return c;
}

static Color pickStarTint(float layer) {
  // Slightly brighter on the near layer.
  unsigned char v = (unsigned char)Clamp(200.0f + 55.0f * layer, 0.0f, 255.0f);
  return (Color){v, v, v, 255};
}

static inline Vector2 playerXZ(const Player *pl) {
  const float x = pl->render.position.x;
  const float z = pl->render.position.z + pl->render.position.offset_z;
  return (Vector2){x, z};
}

static void randomizeParticle(ParallaxField *f, ParallaxParticle *pp,
                              unsigned int *rng, const Camera3D *cam,
                              bool ahead, Vector2 dirXZ) {
  // Depth bucket (far / mid / near)
  float u = frand01(rng);
  if (u < 0.34f)
    pp->layer = 0.15f + 0.15f * frand01(rng);
  else if (u < 0.68f)
    pp->layer = 0.40f + 0.15f * frand01(rng);
  else
    pp->layer = 0.75f + 0.20f * frand01(rng);

  // Visuals (tuned for visibility in top-down)
  pp->baseSpeed = 6.0f + 24.0f * pp->layer;
  pp->size = 0.10f + 0.18f * pp->layer; // bigger by default
  pp->alpha = 0.85f;                    // bright baseline
  pp->jitterAmp = 0.01f * pp->layer;
  pp->jitterFreq = 1.0f + 3.0f * frand01(rng);
  pp->stretch = (pp->layer > 0.65f) ? (0.25f + 0.75f * frand01(rng))
                                    : (0.05f + 0.20f * frand01(rng));
  pp->phase = frand01(rng) * 1000.0f;
  pp->tint = pickStarTint(pp->layer);
  pp->streak = 0.0f;

  // Spawn within an overscanned XZ rectangle around the camera.
  Vector3 cpos = cam->position;
  float hx = f->halfExtentXZ.x, hz = f->halfExtentXZ.y;
  float mx = hx + f->respawnMargin, mz = hz + f->respawnMargin;

  if (ahead && (dirXZ.x != 0 || dirXZ.y != 0)) {
    Vector2 n = Vector2Normalize(dirXZ);
    pp->pos.x = cpos.x + n.x * mx + (frand01(rng) - 0.5f) * hx * 0.5f;
    pp->pos.z = cpos.z + n.y * mz + (frand01(rng) - 0.5f) * hz * 0.5f;
  } else {
    pp->pos.x = cpos.x + (frand01(rng) * 2.0f - 1.0f) * mx;
    pp->pos.z = cpos.z + (frand01(rng) * 2.0f - 1.0f) * mz;
  }

  // Fixed Y: starfield plane under the scene.
  const float yPlane = (pp->layer < 0.35f)   ? f->yFar
                       : (pp->layer < 0.70f) ? f->yMid
                                             : f->yNear;
  pp->pos.y = yPlane;
}

// -------------------- public API --------------------

ParallaxField parallaxInit(int particleCount, Vector2 halfExtentXZ,
                           unsigned int seed) {
  ParallaxField f = (ParallaxField){0};

  f.count = particleCount;
  f.halfExtentXZ = halfExtentXZ;

  // Top-down: put stars "under" the scene on a fixed Y plane.
  f.yFar = -2.0f;
  f.yMid = -2.0f;
  f.yNear = -2.0f;
  f.respawnMargin = 24.0f; // softer wrap edges

  f.dotTex = makeDotTexture();

  // Default motion tuning
  f.baseForwardSpeedZ = +28.0f; // constant forward flow (+Z)
  f.playerInfluence = 0.25f;    // tamed player coupling
  f.refVelForStreaks = 40.0f;   // |scroll| mapped to full streaks
  f.speedScale = 1.0f;

  // Player velocity filtering and clamp
  f.velSmoothing = 0.15f;    // 0=no filter, 1=very slow
  f.maxInfluenceVel = 22.0f; // clamp player contribution

  f.hasPrevPlayerPos = false;
  f.prevPlayerXZ = (Vector2){0, 0};
  f.smoothedVelXZ = (Vector2){0, 0};
  f.time = 0.0f;

  f.p = (ParallaxParticle *)MemAlloc(sizeof(ParallaxParticle) *
                                     (size_t)particleCount);

  Camera3D fakeCam = {0};
  fakeCam.position = (Vector3){0, 0, 0};

  unsigned int rng = seed ? seed : 0xCAFEBABE;
  for (int i = 0; i < particleCount; ++i) {
    randomizeParticle(&f, &f.p[i], &rng, &fakeCam, false, (Vector2){0, 0});
  }

  return f;
}

void destroyParallax(ParallaxField *f) {
  if (!f)
    return;
  if (f->dotTex.id)
    UnloadTexture(f->dotTex);
  if (f->p) {
    MemFree(f->p);
    f->p = NULL;
  }
  f->count = 0;
}

void parallaxUpdate(ParallaxField *f, const Camera3D *cam,
                    const Player *player) {
  if (!f || !cam || !player)
    return;

  const float dt = GetFrameTime();
  f->time += dt;

  // Raw player velocity in XZ
  const Vector2 curr = playerXZ(player);
  Vector2 pvel = (Vector2){0, 0};
  if (!f->hasPrevPlayerPos) {
    f->prevPlayerXZ = curr;
    f->hasPrevPlayerPos = true;
  } else if (dt > 0.0f) {
    pvel.x = (curr.x - f->prevPlayerXZ.x) / dt;
    pvel.y = (curr.y - f->prevPlayerXZ.y) / dt;
    f->prevPlayerXZ = curr;
  }

  // Low-pass filter + clamp to tame player influence
  float a = Clamp(f->velSmoothing, 0.0f, 1.0f);
  f->smoothedVelXZ.x = Lerp(pvel.x, f->smoothedVelXZ.x, a);
  f->smoothedVelXZ.y = Lerp(pvel.y, f->smoothedVelXZ.y, a);

  float len = Vector2Length(f->smoothedVelXZ);
  if (len > f->maxInfluenceVel && len > 0.0f) {
    f->smoothedVelXZ = Vector2Scale(f->smoothedVelXZ, f->maxInfluenceVel / len);
  }

  // Scroll vector: forward flow + opposite-to-player motion
  Vector2 scroll = (Vector2){0.0f, f->baseForwardSpeedZ};
  scroll.x += -f->playerInfluence * f->smoothedVelXZ.x;
  scroll.y += -f->playerInfluence * f->smoothedVelXZ.y;

  float speedMag = Vector2Length(scroll);
  Vector2 dirXZ = (speedMag > 0.0001f) ? Vector2Scale(scroll, 1.0f / speedMag)
                                       : (Vector2){0.0f, 1.0f}; // forward +Z

  // Per-particle update
  const float hx = f->halfExtentXZ.x, hz = f->halfExtentXZ.y;
  const float mx = hx + f->respawnMargin, mz = hz + f->respawnMargin;
  const Vector3 cpos = cam->position;

  const float ref = (f->refVelForStreaks > 0.0f) ? f->refVelForStreaks : 40.0f;
  const float vel01 = clamp01(speedMag / ref);

  static float tAccum = 0.0f;
  tAccum += dt;

  for (int i = 0; i < f->count; ++i) {
    ParallaxParticle *pp = &f->p[i];

    // Fixed Y on the starfield plane (under the scene)
    const float planeY = (pp->layer < 0.35f)   ? f->yFar
                         : (pp->layer < 0.70f) ? f->yMid
                                               : f->yNear;
    pp->pos.y = planeY;

    // Move in XZ with depth factor
    const float layerFactor = 0.15f + 0.85f * pp->layer;
    const Vector2 v =
        Vector2Scale(dirXZ, pp->baseSpeed * f->speedScale * layerFactor);
    pp->pos.x += v.x * dt;
    pp->pos.z += v.y * dt;

    // Subtle jitter
    if (pp->jitterAmp > 0.0f) {
      float s = sinf(pp->phase + tAccum * pp->jitterFreq);
      float c =
          cosf(pp->phase * 0.7f + tAccum * (pp->jitterFreq * 0.6f + 0.3f));
      pp->pos.x += s * pp->jitterAmp;
      pp->pos.z += c * pp->jitterAmp * 0.5f;
    }

    // Toroidal wrap-around around camera box +/- mx,mz
    bool wrapped = false;
    if (pp->pos.x < cpos.x - mx) {
      pp->pos.x += 2.0f * mx;
      wrapped = true;
    } else if (pp->pos.x > cpos.x + mx) {
      pp->pos.x -= 2.0f * mx;
      wrapped = true;
    }

    if (pp->pos.z < cpos.z - mz) {
      pp->pos.z += 2.0f * mz;
      wrapped = true;
    } else if (pp->pos.z > cpos.z + mz) {
      pp->pos.z -= 2.0f * mz;
      wrapped = true;
    }

    if (wrapped) {
      // Tiny re-randomization to avoid visible tiling
      unsigned int rng = (unsigned int)(pp->pos.x * 131.0f) ^
                         (unsigned int)(pp->pos.z * 911.0f);
      pp->alpha = 0.70f + 0.30f * frand01(&rng);
      pp->phase = frand01(&rng) * 1000.0f;
    }

    // Streak intensity: near layer + scroll speed
    const float layerBoost = smoothstep(0.6f, 1.0f, pp->layer);
    pp->streak = Clamp(pp->stretch * layerBoost * vel01, 0.0f, 1.0f);
  }
}

void parallaxRender(const ParallaxField *f, const Camera3D *cam) {
  if (!f || !cam)
    return;

  // Alpha blending for maximum visibility; switch to BLEND_ADDITIVE for glow.
  BeginBlendMode(BLEND_ALPHA);

  // World up works well for top-down too.
  const Vector3 upBill = (Vector3){0, 1, 0};

  for (int i = 0; i < f->count; ++i) {
    const ParallaxParticle *pp = &f->p[i];

    const float base = pp->size;
    const Vector2 size = (Vector2){base, base * (1.0f + pp->streak * 6.0f)};
    const Rectangle src =
        (Rectangle){0, 0, (float)f->dotTex.width, (float)f->dotTex.height};
    const Color tint = colorWithAlpha(pp->tint, pp->alpha);

    // rotation = 0; we rely on 'upBill' only.
    DrawBillboardPro(*cam, f->dotTex, src, pp->pos, upBill, size,
                     (Vector2){0.5f, 0.5f}, 0.0f, tint);
  }

  EndBlendMode();
}
