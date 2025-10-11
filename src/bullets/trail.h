#include "raylib.h"
#include "rlgl.h"
#include <raymath.h>
#include <stdbool.h>
#include <stddef.h>

// Maximum number of trail particles
#define TRAIL_MAX 128

/**
 * @brief Represents a single particle in the trail effect.
 */
typedef struct {
  Vector3 pos, vel; /// Position and velocity
  float size;       /// Current size
  float rot;        /// Rotation angle
  float life, ttl;  /// Remaining life and total lifespan
  Color color;      /// Color with alpha
} TrailParticle;

/**
 * @brief Emitter for generating and managing trail particles.
 */
typedef struct {
  TrailParticle p[TRAIL_MAX]; /// Array of particles
  int count;                  /// Current active particle count
  Texture2D tex;              /// Texture for particles
  bool additive;              /// Blending mode
  float spawnRate;            /// Particles spawned per second
  float accum;                /// Accumulated spawn time
  float baseSize;             /// Base size of particles
  float grow;                 /// Growth rate of particles
  float damping;              /// Velocity damping factor
  float speed;                /// Initial speed of particles
  Color baseColor;            /// Base color of particles
} TrailEmitter;

/**
 * @brief Creates and initializes a new TrailEmitter instance.
 *
 * @param tex The texture to use for the trail particles.
 * @param additive Whether to use additive blending (true) or alpha blending (false).
 * @return A fully initialized TrailEmitter object.
 */
TrailEmitter newTrailEmitter(Texture2D tex, bool additive);

/**
 * @brief Emits new trail particles from the emitter at the specified origin and direction.
 *
 * @param emitter Pointer to the TrailEmitter instance.
 * @param origin The 3D position from which to emit particles.
 * @param dir The direction vector for particle emission.
 * @param dt Time elapsed since the last update (in seconds).
 */
void trailEmit(TrailEmitter *e, Vector3 origin, Vector3 dir, float dt);

/**
 * @brief Updates the state of all active trail particles in the emitter.
 *
 * @param e Pointer to the TrailEmitter instance.
 * @param dt Time elapsed since the last update (in seconds).
 */
void trailUpdate(TrailEmitter *e, float dt);

/**
 * @brief Renders all active trail particles using the specified camera.
 *
 * @param e Pointer to the TrailEmitter instance.
 * @param cam The Camera3D used for rendering the scene.
 */
void trailDraw(TrailEmitter *e, Camera3D cam);