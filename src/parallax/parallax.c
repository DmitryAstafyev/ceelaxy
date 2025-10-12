// parallax.c
// ================================================
// AI usage note: This module was developed based on AI-generated (ChatGPT) code.
// The author made additional changes and performed partial refactoring.
// ================================================

#include "parallax.h"
#include "../units/player.h"

#include <limits.h>
#include <math.h>
#include <stdlib.h>

// -------------------- internal utils --------------------
static inline float clamp01(float x) { return x < 0 ? 0 : (x > 1 ? 1 : x); }

static inline float smoothstep(float e0, float e1, float x)
{
  if (e0 == e1)
    return x >= e1 ? 1.0f : 0.0f;
  x = clamp01((x - e0) / (e1 - e0));
  return x * x * (3.0f - 2.0f * x);
}

static inline float frand01(unsigned int *state)
{
  *state = (*state * 1664525u + 1013904223u);
  return (float)(*state) / (float)UINT_MAX;
}

// Create a simple 1x1 white texture for point sprites.
static Texture2D makeDotTexture(void)
{
  // 1x1 white tex; swap to a round sprite if you have one.
  Image img = GenImageColor(1, 1, WHITE);
  Texture2D t = LoadTextureFromImage(img);
  UnloadImage(img);
  return t;
}

// Apply alpha to a Color, clamping to [0..255].
static Color colorWithAlpha(Color c, float a)
{
  c.a = (unsigned char)Clamp(a * 255.0f, 0.0f, 255.0f);
  return c;
}

// Slightly bluish white for distant stars, pure white for near ones.
static Color pickStarTint(float layer)
{
  // Slightly brighter on the near layer.
  unsigned char v = (unsigned char)Clamp(200.0f + 55.0f * layer, 0.0f, 255.0f);
  return (Color){v, v, v, 255};
}

// -------------------- internal logic --------------------
static inline Vector2 playerXZ(const Player *pl)
{
  const float x = pl->render.position.x;
  const float z = pl->render.position.z + pl->render.position.offset_z;
  return (Vector2){x, z};
}

// Update smoothed player velocity, return the influence vector.
static void randomizeParticle(ParallaxField *field, ParallaxParticle *pp,
                              unsigned int *rng, const Camera3D *cam,
                              bool ahead, Vector2 dirXZ)
{
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
  float hx = field->halfExtentXZ.x, hz = field->halfExtentXZ.y;
  float mx = hx + field->respawnMargin, mz = hz + field->respawnMargin;

  if (ahead && (dirXZ.x != 0 || dirXZ.y != 0))
  {
    Vector2 n = Vector2Normalize(dirXZ);
    pp->pos.x = cpos.x + n.x * mx + (frand01(rng) - 0.5f) * hx * 0.5f;
    pp->pos.z = cpos.z + n.y * mz + (frand01(rng) - 0.5f) * hz * 0.5f;
  }
  else
  {
    pp->pos.x = cpos.x + (frand01(rng) * 2.0f - 1.0f) * mx;
    pp->pos.z = cpos.z + (frand01(rng) * 2.0f - 1.0f) * mz;
  }

  // Fixed Y: starfield plane under the scene.
  const float yPlane = (pp->layer < 0.35f)   ? field->yFar
                       : (pp->layer < 0.70f) ? field->yMid
                                             : field->yNear;
  pp->pos.y = yPlane;
}

// -------------------- public API --------------------

/** Initializes a parallax starfield effect.
 *
 * @param particleCount Number of particles to create.
 * @param halfExtentXZ Half-size of the field in X and Z directions (world units).
 * @param seed Random seed for particle distribution.
 * @return Initialized ParallaxField structure.
 */
ParallaxField parallaxInit(int particleCount, Vector2 halfExtentXZ,
                           unsigned int seed)
{
  ParallaxField field = (ParallaxField){0};

  field.count = particleCount;
  field.halfExtentXZ = halfExtentXZ;

  // Top-down: put stars "under" the scene on a fixed Y plane.
  field.yFar = -2.0f;
  field.yMid = -2.0f;
  field.yNear = -2.0f;
  field.respawnMargin = 24.0f; // softer wrap edges

  field.dotTex = makeDotTexture();

  // Default motion tuning
  field.baseForwardSpeedZ = +28.0f; // constant forward flow (+Z)
  field.playerInfluence = 0.25f;    // tamed player coupling
  field.refVelForStreaks = 40.0f;   // |scroll| mapped to full streaks
  field.speedScale = 1.0f;

  // Player velocity filtering and clamp
  field.velSmoothing = 0.15f;    // 0=no filter, 1=very slow
  field.maxInfluenceVel = 22.0f; // clamp player contribution

  field.hasPrevPlayerPos = false;
  field.prevPlayerXZ = (Vector2){0, 0};
  field.smoothedVelXZ = (Vector2){0, 0};
  field.time = 0.0f;

  size_t count = (size_t)particleCount;
  size_t bytes = count * sizeof(ParallaxParticle);

  if (bytes > UINT_MAX)
  {
    bytes = UINT_MAX;
  }

  field.p = (ParallaxParticle *)MemAlloc((unsigned int)bytes);

  Camera3D fakeCam = {0};
  fakeCam.position = (Vector3){0, 0, 0};

  unsigned int rng = seed ? seed : 0xCAFEBABE;
  for (int i = 0; i < particleCount; ++i)
  {
    randomizeParticle(&field, &field.p[i], &rng, &fakeCam, false, (Vector2){0, 0});
  }

  return field;
}

/** Advances the parallax field state for the current frame.
 *
 * @param field Pointer to the ParallaxField to update.
 * @param cam Pointer to the active Camera3D for view/projection.
 * @param player Pointer to the Player for velocity influence (can be NULL).
 */
void destroyParallax(ParallaxField *field)
{
  if (!field)
    return;
  if (field->dotTex.id)
    UnloadTexture(field->dotTex);
  if (field->p)
  {
    MemFree(field->p);
    field->p = NULL;
  }
  field->count = 0;
}

/** Renders the parallax field as a background effect.
 *
 * @param field Pointer to the ParallaxField to render.
 * @param cam Pointer to the active Camera3D for view/projection.
 */
void parallaxUpdate(ParallaxField *field, const Camera3D *cam,
                    const Player *player)
{
  if (!field || !cam || !player)
    return;

  const float dt = GetFrameTime();
  field->time += dt;

  // Raw player velocity in XZ
  const Vector2 curr = playerXZ(player);
  Vector2 pvel = (Vector2){0, 0};
  if (!field->hasPrevPlayerPos)
  {
    field->prevPlayerXZ = curr;
    field->hasPrevPlayerPos = true;
  }
  else if (dt > 0.0f)
  {
    pvel.x = (curr.x - field->prevPlayerXZ.x) / dt;
    pvel.y = (curr.y - field->prevPlayerXZ.y) / dt;
    field->prevPlayerXZ = curr;
  }

  // Low-pass filter + clamp to tame player influence
  float a = Clamp(field->velSmoothing, 0.0f, 1.0f);
  field->smoothedVelXZ.x = Lerp(pvel.x, field->smoothedVelXZ.x, a);
  field->smoothedVelXZ.y = Lerp(pvel.y, field->smoothedVelXZ.y, a);

  float len = Vector2Length(field->smoothedVelXZ);
  if (len > field->maxInfluenceVel && len > 0.0f)
  {
    field->smoothedVelXZ = Vector2Scale(field->smoothedVelXZ, field->maxInfluenceVel / len);
  }

  // Scroll vector: forward flow + opposite-to-player motion
  Vector2 scroll = (Vector2){0.0f, field->baseForwardSpeedZ};
  scroll.x += -field->playerInfluence * field->smoothedVelXZ.x;
  scroll.y += -field->playerInfluence * field->smoothedVelXZ.y;

  float speedMag = Vector2Length(scroll);
  Vector2 dirXZ = (speedMag > 0.0001f) ? Vector2Scale(scroll, 1.0f / speedMag)
                                       : (Vector2){0.0f, 1.0f}; // forward +Z

  // Per-particle update
  const float hx = field->halfExtentXZ.x, hz = field->halfExtentXZ.y;
  const float mx = hx + field->respawnMargin, mz = hz + field->respawnMargin;
  const Vector3 cpos = cam->position;

  const float ref = (field->refVelForStreaks > 0.0f) ? field->refVelForStreaks : 40.0f;
  const float vel01 = clamp01(speedMag / ref);

  static float tAccum = 0.0f;
  tAccum += dt;

  for (int i = 0; i < field->count; ++i)
  {
    ParallaxParticle *pp = &field->p[i];

    // Fixed Y on the starfield plane (under the scene)
    const float planeY = (pp->layer < 0.35f)   ? field->yFar
                         : (pp->layer < 0.70f) ? field->yMid
                                               : field->yNear;
    pp->pos.y = planeY;

    // Move in XZ with depth factor
    const float layerFactor = 0.15f + 0.85f * pp->layer;
    const Vector2 v =
        Vector2Scale(dirXZ, pp->baseSpeed * field->speedScale * layerFactor);
    pp->pos.x += v.x * dt;
    pp->pos.z += v.y * dt;

    // Subtle jitter
    if (pp->jitterAmp > 0.0f)
    {
      float s = sinf(pp->phase + tAccum * pp->jitterFreq);
      float c =
          cosf(pp->phase * 0.7f + tAccum * (pp->jitterFreq * 0.6f + 0.3f));
      pp->pos.x += s * pp->jitterAmp;
      pp->pos.z += c * pp->jitterAmp * 0.5f;
    }

    // Toroidal wrap-around around camera box +/- mx,mz
    bool wrapped = false;
    if (pp->pos.x < cpos.x - mx)
    {
      pp->pos.x += 2.0f * mx;
      wrapped = true;
    }
    else if (pp->pos.x > cpos.x + mx)
    {
      pp->pos.x -= 2.0f * mx;
      wrapped = true;
    }

    if (pp->pos.z < cpos.z - mz)
    {
      pp->pos.z += 2.0f * mz;
      wrapped = true;
    }
    else if (pp->pos.z > cpos.z + mz)
    {
      pp->pos.z -= 2.0f * mz;
      wrapped = true;
    }

    if (wrapped)
    {
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

/** Renders the parallax field as a background effect.
 *
 * @param field Pointer to the ParallaxField to render.
 * @param cam Pointer to the active Camera3D for view/projection.
 */
void parallaxRender(const ParallaxField *field, const Camera3D *cam)
{
  if (!field || !cam)
    return;

  // Alpha blending for maximum visibility; switch to BLEND_ADDITIVE for glow.
  BeginBlendMode(BLEND_ALPHA);

  // World up works well for top-down too.
  const Vector3 upBill = (Vector3){0, 1, 0};

  for (int i = 0; i < field->count; ++i)
  {
    const ParallaxParticle *pp = &field->p[i];

    const float base = pp->size;
    const Vector2 size = (Vector2){base, base * (1.0f + pp->streak * 6.0f)};
    const Rectangle src =
        (Rectangle){0, 0, (float)field->dotTex.width, (float)field->dotTex.height};
    const Color tint = colorWithAlpha(pp->tint, pp->alpha);

    // rotation = 0; we rely on 'upBill' only.
    DrawBillboardPro(*cam, field->dotTex, src, pp->pos, upBill, size,
                     (Vector2){0.5f, 0.5f}, 0.0f, tint);
  }

  EndBlendMode();
}
