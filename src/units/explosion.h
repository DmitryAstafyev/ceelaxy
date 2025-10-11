// explosion.h

#pragma once
#include "raylib.h"

#define EXP_MAX 256

/**
 * @brief Different kinds of explosion particles.
 */
typedef enum
{
  EXP_FIRE = 0,
  EXP_SMOKE = 1,
  EXP_SPARK = 2
} ExpKind;

/**
 * @brief Represents a single explosion particle.
 */
typedef struct
{
  Vector3 pos, vel; /// position and velocity
  float size, rot;  /// size and rotation
  float life, ttl;  /// remaining life and total lifespan
  ExpKind kind;     /// particle type (fire, smoke, spark)
  Color color;      /// color with alpha
} ExpParticle;

/**
 * @brief Manages a collection of explosion particles and their behavior.
 */
typedef struct
{
  ExpParticle p[EXP_MAX]; /// array of particles
  int count;              /// current active particle count
  bool active;            /// false until the first update-spawn
  Vector3 spawn_origin;   /// origin at spawn moment
  Vector3 last_origin;    /// origin from previous frame (to compute delta)
  float gravityY;         /// downward sink
  float damping;          /// velocity damping
  float backDrift;        /// drift along camera forward (m/s)
  float carrySmoke;       /// how much smoke follows moving ship [0..1]
  float carryFire;        /// how much fire follows
  float carrySpark;       /// how much sparks follow
  Texture2D texFire;      /// fire texture
  Texture2D texSmoke;     /// smoke texture
  Texture2D texGlow;      // optional halo
} BulletExplosion;

/**
 * @brief Creates and initializes a new BulletExplosion instance.
 *
 * @param fire Texture for fire particles.
 * @param smoke Texture for smoke particles.
 * @param glow Texture for glow/halo effect (can be white).
 * @return A fully initialized BulletExplosion object.
 */
BulletExplosion newBulletExplosion(Texture2D fire, Texture2D smoke,
                                   Texture2D glow);
/**
 * @brief Spawns a burst of explosion particles at the specified origin.
 *
 * This function initializes a set of explosion particles with random
 * velocities, lifespans, sizes, and colors based on the defined particle
 * types (fire, smoke, sparks).
 *
 * @param e Pointer to the BulletExplosion instance.
 * @param origin The 3D position where the explosion should occur.
 * @param cam Pointer to the active Camera3D for orientation.
 */
void bulletExplosionSpawnAt(BulletExplosion *e, Vector3 origin,
                            const Camera3D *cam);
/**
 * @brief Updates the state of all active explosion particles.
 *
 * This function advances the simulation of the explosion particles,
 * updating their positions, lifespans, and spawning new particles
 * as needed. It also handles the effect of gravity and damping.
 *
 * @param e Pointer to the BulletExplosion instance.
 * @param origin Current origin position (for carry effect).
 * @param dt Time elapsed since the last update (in seconds).
 * @param cam Pointer to the active Camera3D for orientation.
 */
void bulletExplosionUpdate(BulletExplosion *e, Vector3 origin, float dt,
                           const Camera3D *cam);
/**
 * @brief Renders all active explosion particles using the specified camera.
 *
 * This function draws each particle as a billboarded quad facing the camera,
 * using the appropriate texture based on the particle type. It also applies
 * color and transparency effects based on the particle's remaining life.
 *
 * @param e Pointer to the BulletExplosion instance.
 * @param cam The Camera3D used for rendering the scene.
 */
void bulletExplosionDraw(BulletExplosion *e, Camera3D cam);

/**
 * @brief Frees all memory used by the BulletExplosion instance.
 *
 * This function unloads the textures associated with the explosion
 * and resets the particle count.
 *
 * @param e Pointer to the BulletExplosion instance to destroy.
 */
static inline bool bulletExplosionIsDead(const BulletExplosion *e)
{
  return !e || e->count == 0;
}
