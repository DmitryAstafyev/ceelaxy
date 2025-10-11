#pragma once
#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>
#include <stddef.h>
#include "../units/player.h"

/**
 * @brief Represents a single particle in the parallax starfield.
 */
typedef struct
{
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

/**
 * @brief Represents the full parallax starfield effect.
 */
typedef struct
{
  ParallaxParticle *p;
  int count;

  Vector2 halfExtentXZ;
  float yFar, yMid, yNear;
  float respawnMargin;

  Texture2D dotTex;

  bool hasPrevPlayerPos;
  Vector2 prevPlayerXZ;
  Vector2 smoothedVelXZ;
  float velSmoothing;
  float maxInfluenceVel;
  float time;

  float baseForwardSpeedZ; // constant forward flow (+Z), world units/s
  float playerInfluence;   // how much player velocity steers the field
  float refVelForStreaks;  // velocity mapped to streaks (units/s)
  float speedScale;        // global multiplier for particle baseSpeed
} ParallaxField;

/** Initializes a parallax starfield effect.
 *
 * @param particleCount Number of particles to create.
 * @param halfExtentXZ Half-size of the field in X and Z directions (world units).
 * @param seed Random seed for particle distribution.
 * @return Initialized ParallaxField structure.
 */
ParallaxField parallaxInit(int particleCount, Vector2 halfExtentXZ,
                           unsigned int seed);

/** Advances the parallax field state for the current frame.
 *
 * @param f Pointer to the ParallaxField to update.
 * @param cam Pointer to the active Camera3D for view/projection.
 * @param player Pointer to the Player for velocity influence (can be NULL).
 */
void parallaxUpdate(ParallaxField *f, const Camera3D *cam,
                    const Player *player);
/** Renders the parallax field as a background effect.
 *
 * @param f Pointer to the ParallaxField to render.
 * @param cam Pointer to the active Camera3D for view/projection.
 */
void parallaxRender(const ParallaxField *f, const Camera3D *cam);

/** Releases all memory used by the parallax field.
 *
 * @param f Pointer to the ParallaxField to destroy.
 */
void destroyParallax(ParallaxField *f);
