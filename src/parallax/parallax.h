#pragma once
#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>
#include <stddef.h>

// Forward declaration to avoid hard coupling in the header.
typedef struct Player Player;

/** Single particle of the parallax field. */
typedef struct {
  Vector3 pos;
  float layer;      // 0..1 depth: 0 = far, 1 = near
  float baseSpeed;  // base world speed factor for this particle
  float size;       // billboard base size (world units)
  float alpha;      // 0..1
  float jitterAmp;  // small X/Z jitter amplitude
  float jitterFreq; // Hz
  float stretch;    // 0..1 potential streak factor
  float streak;     // 0..1 current streak factor (computed)
  Color tint;
  float phase; // random phase for jitter
} ParallaxParticle;

/** Opaque parallax field state. */
typedef struct {
  // Particles
  ParallaxParticle *p;
  int count;

  // World-space extents around the camera for culling/respawn (X,Z half sizes).
  Vector2 halfExtentXZ;
  float yFar, yMid, yNear;
  float respawnMargin;

  // Internal texture (1x1 white) used for billboards.
  Texture2D dotTex;

  // --- Internal runtime state (do not touch directly) ---
  bool hasPrevPlayerPos;
  Vector2 prevPlayerXZ;
  Vector2 smoothedVelXZ;
  float velSmoothing;
  float maxInfluenceVel;
  float time;

  // Tuning (you can modify after init if needed)
  float baseForwardSpeedZ; // constant forward flow (+Z), world units/s
  float playerInfluence;   // how much player velocity steers the field
  float refVelForStreaks;  // velocity mapped to streaks (units/s)
  float speedScale;        // global multiplier for particle baseSpeed
} ParallaxField;

/** Public API (only these three are meant to be used from outside). */
ParallaxField parallaxInit(int particleCount, Vector2 halfExtentXZ,
                           unsigned int seed);
void parallaxUpdate(ParallaxField *f, const Camera3D *cam,
                    const Player *player);
void parallaxRender(const ParallaxField *f, const Camera3D *cam);

/** Optional cleanup (call once when destroying the scene). */
void destroyParallax(ParallaxField *f);
