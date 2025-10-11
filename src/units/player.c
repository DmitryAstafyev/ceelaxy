/**
 * @file player.c
 * @brief Implements player initialization logic, including position, visual
 * state, movement parameters, and rendering configuration.
 */
#include "player.h"
#include "../bullets/bullets.h"
#include "../game/levels.h"
#include "../textures/textures.h"
#include "../utils/debug.h"
#include "unit.h"
#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <wchar.h>

/// Minimum delay (in seconds) between direction changes to reset acceleration.
static const float ACCELERATION_DEALY = 0.2f;

/// Initial acceleration value when direction is changed.
static const float ACCELERATION_INIT = 0.1f;

static const float ACCELERATION_MIN = 0.01f;

/// Acceleration increment when holding a direction key.
static const float ACCELERATION_STEP = 0.05f;

/// Maximum acceleration value allowed.
static const float ACCELERATION_MAX = 1.0f;

/// Maximum rotation angle along the X axis during movement.
static const float MAX_ROTATE_X = 15.0f;

/// Maximum rotation angle along the Y axis (unused).
static const float MAX_ROTATE_Y = 0.0f;

/// Maximum rotation angle along the Z axis during movement.
static const float MAX_ROTATE_Z = 35.0f;

/// Step of rotation applied per frame along the X axis.
static const float STEP_ROTATE_X = 1.0f;

/// Step of rotation along the Y axis (unused).
static const float STEP_ROTATE_Y = 0.0f;

/// Step of rotation applied per frame along the Z axis.
static const float STEP_ROTATE_Z = 2.0f;

PlayerPosition newPlayerPosition(float max_x, float max_y, float max_z,
                                 float offset_z)
{
  PlayerPosition position;
  position.x = 0.0f;
  position.y = 0.0f;
  position.z = 0.0f;
  position.max_x = max_x;
  position.max_y = max_y;
  position.max_z = max_z;
  position.offset_z = offset_z;
  return position;
};

PlayerVisualState newPlayerVisualState(float offset_z)
{
  PlayerVisualState state;
  state.max_rotate_x = MAX_ROTATE_X;
  state.max_rotate_y = MAX_ROTATE_Y;
  state.max_rotate_z = MAX_ROTATE_Z;
  state.rotate_x = 0.0f;
  state.rotate_y = 0.0f;
  state.rotate_z = 0.0f;
  state.rotate_step_x = STEP_ROTATE_X;
  state.rotate_step_y = STEP_ROTATE_Y;
  state.rotate_step_z = STEP_ROTATE_Z;
  state.max_angle = 15.0f;
  state.angle = 0.0f;
  return state;
};

PlayerMovement newPlayerMovement()
{
  PlayerMovement movement;
  movement.last_key_press = GetTime();
  movement.acceleration = 0;
  movement.direction_x_key = 0;
  movement.direction_z_key = 0;
  return movement;
}

PlayerRender newPlayerRender(float max_x, float max_y, float max_z,
                             float offset_z)
{
  PlayerRender render;
  render.position = newPlayerPosition(max_x, max_y, max_z, offset_z);
  render.size = newUnitSize();
  render.state = newPlayerVisualState(offset_z);
  render.movement = newPlayerMovement();
  return render;
}
/**
 * @brief Allocates and initializes a new Player instance.
 *
 * This function creates a new Player object, setting up its position,
 * movement parameters, visual state, model reference, and bullet list.
 * It also initializes the explosion effect for when the player is hit.
 *
 * @param max_x Maximum X boundary for player movement.
 * @param max_y Maximum Y boundary for player movement.
 * @param max_z Maximum Z boundary for player movement.
 * @param offset_z Z offset applied to the player's position for rendering or
 * bullet logic.
 * @param model Pointer to the ShipModel used for rendering the player.
 * @param bullets Pointer to the global BulletList used for firing bullets.
 * @param textures Pointer to the GameTextures containing necessary textures.
 * @return Pointer to the newly created Player instance, or NULL on failure.
 */
Player *newPlayer(float max_x, float max_y, float max_z, float offset_z,
                  ShipModel *model, BulletList *bullets,
                  GameTextures *textures)
{
  if (!model)
  {
    return NULL;
  }
  Player *player = malloc(sizeof(Player));
  if (!player)
  {
    return NULL;
  }
  GameTexture *tex_fire_soft = getGameTextureById(textures, TEX_ID_FIRE_SOFT);
  if (tex_fire_soft == NULL)
  {
    TraceLog(LOG_ERROR, "Fail to find texture: %i", TEX_ID_FIRE_SOFT);
    exit(1);
  }
  GameTexture *tex_smoke_soft = getGameTextureById(textures, TEX_ID_SMOKE_SOFT);
  if (tex_smoke_soft == NULL)
  {
    TraceLog(LOG_ERROR, "Fail to find texture: %i", TEX_ID_SMOKE_SOFT);
    exit(1);
  }
  GameTexture *tex_glow = getGameTextureById(textures, TEX_ID_GLOW);
  if (tex_glow == NULL)
  {
    TraceLog(LOG_ERROR, "Fail to find texture: %i", TEX_ID_GLOW);
    exit(1);
  }
  player->type = UNIT_TYPE_SOLDER;
  player->state = newUnitState();
  player->render = newPlayerRender(max_x, max_y, max_z, offset_z);
  player->model = model;
  player->bullets = bullets;
  player->hit = NULL;
  player->explosion_bullet = newBulletExplosion(
      tex_fire_soft->tex, tex_smoke_soft->tex, tex_glow->tex);
  return player;
}

// Returns true if the movement direction has changed since the last frame.
bool directionChanged(Player *player)
{
  return (IsKeyDown(KEY_LEFT) &&
          player->render.movement.direction_x_key != KEY_LEFT) ||
         (IsKeyDown(KEY_RIGHT) &&
          player->render.movement.direction_x_key != KEY_RIGHT) ||
         (IsKeyDown(KEY_UP) &&
          player->render.movement.direction_z_key != KEY_UP) ||
         (IsKeyDown(KEY_DOWN) &&
          player->render.movement.direction_z_key != KEY_DOWN);
}

/**
 * @brief Updates the player's position, movement, and shooting based on input.
 *
 * This function processes keyboard input to move the player within defined
 * boundaries, applies acceleration and rotation effects, and handles bullet
 * firing with a cooldown based on the current level parameters.
 *
 * @param player Pointer to the Player instance to update.
 * @param level Pointer to the current Level containing player parameters.
 * @param textures Pointer to the GameTextures for bullet creation.
 */
void updatePlayer(Player *player, Level *level, GameTextures *textures)
{
  if (!player)
  {
    return;
  }

  PlayerMovement *movement = &player->render.movement;
  PlayerVisualState *state = &player->render.state;
  PlayerPosition *position = &player->render.position;
  BulletList *bullets = player->bullets;

  double current_time = GetTime();
  double elapsed_last_bullet_spawn = current_time - bullets->last_spawn;

  if (IsKeyDown(KEY_SPACE) &&
      elapsed_last_bullet_spawn > level->player.bullet_delay_spawn)
  {
    Bullet bullet =
        newBullet(BULLET_MOVEMENT_DIRECTION_UP,
                  newBulletPosition(position->x, position->y,
                                    position->z + position->offset_z),
                  newBulletSize(0.25f, .25f, 2.0f),
                  newBulletParameters(level->player.damage_life,
                                      level->player.damage_energy),
                  BULLET_OWNER_PLAYER, level->player.bullet_acceleration,
                  level->player.bullet_init_speed, textures);
    insertBulletIntoList(player->bullets, bullet);
    bullets->last_spawn = current_time;
  }
  if (!(IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_UP) ||
        IsKeyDown(KEY_DOWN)))
  {
    if (state->rotate_x != 0)
    {
      if (state->rotate_x < 0)
      {
        state->rotate_x += state->rotate_step_x;
      }
      else
      {
        state->rotate_x -= state->rotate_step_x;
      }
    }
    if (state->rotate_z != 0)
    {
      if (state->rotate_z < 0)
      {
        state->rotate_z += state->rotate_step_z;
      }
      else
      {
        state->rotate_z -= state->rotate_step_z;
      }
    }
    return;
  }
  double elapsed = current_time - movement->last_key_press;
  movement->last_key_press = current_time;
  float energy_factor =
      ((float)player->state.energy / player->state.init_energy);
  if (elapsed > ACCELERATION_DEALY || directionChanged(player))
  {
    movement->acceleration = ACCELERATION_INIT;
  }
  else
  {
    movement->acceleration +=
        ACCELERATION_MIN + ACCELERATION_STEP * energy_factor;
  }

  if (movement->acceleration > ACCELERATION_MAX)
  {
    movement->acceleration = ACCELERATION_MAX;
  }

  if (IsKeyDown(KEY_LEFT))
  {
    position->x -= movement->acceleration;
    movement->direction_x_key = KEY_LEFT;
    state->rotate_z -= state->rotate_step_z;
    state->rotate_z = fabsf(state->rotate_z) >= state->max_rotate_z
                          ? -state->max_rotate_z
                          : state->rotate_z;
  }
  if (IsKeyDown(KEY_RIGHT))
  {
    position->x += movement->acceleration;
    movement->direction_x_key = KEY_RIGHT;
    state->rotate_z += state->rotate_step_z;
    state->rotate_z = fabsf(state->rotate_z) >= state->max_rotate_z
                          ? state->max_rotate_z
                          : state->rotate_z;
  }
  position->x =
      copysignf(fminf(fabsf(position->x), fabsf(position->max_x)), position->x);
  if (IsKeyDown(KEY_UP))
  {
    position->z -= movement->acceleration;
    movement->direction_z_key = KEY_UP;
    state->rotate_x += state->rotate_step_x;
    state->rotate_x = fabsf(state->rotate_x) >= state->max_rotate_x
                          ? state->max_rotate_x
                          : state->rotate_x;
  }
  if (IsKeyDown(KEY_DOWN))
  {
    position->z += movement->acceleration;
    movement->direction_z_key = KEY_DOWN;
    state->rotate_x -= state->rotate_step_x;
    state->rotate_x = fabsf(state->rotate_x) >= state->max_rotate_x
                          ? -state->max_rotate_x
                          : state->rotate_x;
  }
  position->z = position->z > 0                 ? 0
                : position->z < position->max_z ? position->max_z
                                                : position->z;
}

/**
 * @brief Computes the world-space bounding box of the player, accounting for
 * rotation.
 *
 * This function calculates the axis-aligned bounding box (AABB) that
 * encapsulates the player's model in world space, taking into account its
 * current position and rotation. This is useful for collision detection and
 * rendering debug information.
 *
 * @param player Pointer to the Player instance.
 * @return BoundingBox representing the player's world-space AABB.
 */
BoundingBox getPlayerBoundingBox(Player *player)
{
  const ShipBoundingBox *box = &player->model->box;
  const PlayerRender *render = &player->render;

  Vector3 pos = {render->position.x, render->position.y,
                 render->position.z + render->position.offset_z};

  const float hx = box->by_x * 0.5f;
  const float hy = box->by_y * 0.5f;
  const float hz = box->by_z * 0.5f;

  Vector3 corners[8] = {
      {-hx, -hy, -hz},
      {-hx, -hy, hz},
      {-hx, hy, -hz},
      {-hx, hy, hz},
      {hx, -hy, -hz},
      {hx, -hy, hz},
      {hx, hy, -hz},
      {hx, hy, hz},
  };

  float rx = render->state.rotate_x;
  float ry = render->state.rotate_y;
  float rz = render->state.rotate_z;

  Matrix R = MatrixRotateXYZ((Vector3){rx, ry, rz});

  Vector3 p0 = Vector3Add(Vector3Transform(corners[0], R), pos);
  BoundingBox world = {.min = p0, .max = p0};

  for (int i = 1; i < 8; ++i)
  {
    Vector3 pw = Vector3Add(Vector3Transform(corners[i], R), pos);
    world.min = Vector3Min(world.min, pw);
    world.max = Vector3Max(world.max, pw);
  }

  return world;
}

/**
 * @brief Renders the player model, hit effects, and explosion effects.
 *
 * This function updates the player's state, applies hit effects if the
 * player was recently hit, and draws the player's 3D model with appropriate
 * transformations. It also handles rendering of explosion effects and debug
 * bounding boxes if enabled.
 *
 * @param player Pointer to the Player instance to render.
 * @param level Pointer to the current Level containing player parameters.
 * @param textures Pointer to the GameTextures for rendering effects.
 * @param camera Pointer to the active Camera3D for view/projection.
 * @param sprites Pointer to the SpriteSheetList for hit animations.
 */
void drawPlayer(Player *player, Level *level, GameTextures *textures,
                Camera3D *camera, SpriteSheetList *sprites)
{
  if (!player)
    return;
  updatePlayer(player, level, textures);

  float dt = GetFrameTime();
  double current = GetTime();
  bool hit = current > BULLET_HIT_SEN_TIME &&
             current - player->state.hit_time < BULLET_HIT_SEN_TIME;

  Vector3 pos = {player->render.position.x, player->render.position.y,
                 player->render.position.z + player->render.position.offset_z};

  if (hit)
  {
    setShipModelColor(player->model, RED);
    if (!player->hit)
    {
      player->hit = newSpriteSheetState(&sprites->tail->self, 1, 3.0f, 0.1f);
    }
    dropSpriteSheetState(player->hit);
    bulletExplosionSpawnAt(&player->explosion_bullet, pos, camera);
  }
  bulletExplosionUpdate(&player->explosion_bullet, pos, dt, camera);
  bulletExplosionDraw(&player->explosion_bullet, *camera);
  drawSpriteSheetState(player->hit, *camera, pos);

  Matrix transform = MatrixTranslate(pos.x, pos.y, pos.z);

  Matrix rotX = MatrixRotateX(DEG2RAD * player->render.state.rotate_x);

  Matrix rotZ = MatrixRotateZ(DEG2RAD * player->render.state.rotate_z);

  Matrix rotY = MatrixRotateY(DEG2RAD * 180.0f);

  Matrix result = MatrixMultiply(rotX, rotZ);
  result = MatrixMultiply(result, rotY);
  result = MatrixMultiply(result, transform);
  Model model = player->model->model;
  model.transform = result;
  DrawModel(model, (Vector3){0, 0, 0}, 1.0f, WHITE);
  if (hit)
  {
    setShipModelColor(player->model, WHITE);
  }
  if (is_debug_mode)
  {
    if (is_debug_mode && player->model->box_model)
    {
      Model box_model = *player->model->box_model;
      box_model.transform = result;
      DrawModel(box_model, (Vector3){0, 0, 0}, 1.0f, RED);
    }
  }
}

/**
 * @brief Frees the memory allocated for a Player instance.
 *
 * This function releases the memory used by the Player object. It does not
 * free any associated resources like models or textures, which should be
 * managed separately.
 *
 * @param player Pointer to the Player instance to destroy.
 */
void destroyPlayer(Player *player)
{
  if (!player)
  {
    return;
  }
  free(player);
}

/**
 * @brief Checks if the player is within the firing line of an enemy unit.
 *
 * This function determines if the player's X position falls within a
 * specified range (factor) of the enemy's X position, indicating that the
 * enemy can fire at the player.
 *
 * @param enemy Pointer to the enemy Unit.
 * @param player Pointer to the Player instance.
 * @param factor The horizontal range around the enemy's X position.
 * @return true if the player is within the firing line, false otherwise.
 */
bool isPlayerOnFireLine(Unit *enemy, Player *player, float factor)
{
  if (!enemy || !player)
    return false;

  const float l_x = enemy->render.position.x - factor;
  const float r_x = enemy->render.position.x + factor;
  return player->render.position.x > l_x && player->render.position.x < r_x;
}

/**
 * @brief Iterates through enemy units and makes them fire at the player if
 * aligned.
 *
 * This function checks each enemy unit in the provided list to see if the
 * player is within its firing line. If so, and if the unit is able to fire
 * based on its cooldown, it spawns a bullet aimed at the player's current
 * position.
 *
 * @param list Pointer to the UnitList containing enemy units.
 * @param camera Pointer to the active Camera3D for view/projection.
 * @param player Pointer to the Player instance.
 * @param level Pointer to the current Level containing unit parameters.
 * @param factor The horizontal range around each enemy's X position for firing.
 * @param textures Pointer to the GameTextures for bullet creation.
 */
void selectUnitsToFire(UnitList *list, Camera3D *camera, Player *player,
                       Level *level, float factor, GameTextures *textures)
{
  UnitNode *node = list->head;
  for (int i = 0; i < list->length; i += 1)
  {
    if (!node)
    {
      break;
    }
    if (isPlayerOnFireLine(&node->self, player, factor) &&
        isUnitAbleToFire(list, &node->self))
    {
      spawnUnitShoot(player->bullets, &node->self, player->render.position.x,
                     player->render.position.z +
                         player->render.position.offset_z,
                     level, textures);
    }
    node = node->next;
  }
}

/**
 * @brief Checks for collisions between player and enemy bullets, applying
 * damage if hit.
 *
 * This function iterates through the global bullet list, checking for
 * collisions with the player's bounding box. If a collision is detected,
 * the bullet is marked as inactive, and the player's health and energy are
 * reduced based on the bullet's parameters. The function also updates the
 * game statistics to record the hit.
 *
 * @param player Pointer to the Player instance.
 * @param bullets Pointer to the global BulletList containing all bullets.
 * @param stat Pointer to the GameStat for recording hits.
 */
void checkBulletHitsPlayer(Player *player, BulletList *bullets,
                           GameStat *stat)
{
  if (!player || !bullets || !stat)
  {
    return;
  }

  BoundingBox playerBox = getPlayerBoundingBox(player);

  BulletNode *node = bullets->head;
  while (node)
  {
    Bullet *bullet = &node->self;
    if (!bullet->alive || bullet->owner != BULLET_OWNER_UNIT)
    {
      node = node->next;
      continue;
    }

    BoundingBox bulletBox = getBulletBoundingBox(bullet);

    if (CheckCollisionBoxes(playerBox, bulletBox))
    {
      bullet->alive = false;
      if (player->state.health > 0)
      {
        player->state.health =
            (player->state.health > bullet->params.health)
                ? (uint8_t)(player->state.health - bullet->params.health)
                : 0u;
      }
      if (player->state.energy > 0)
      {
        player->state.energy =
            (player->state.energy > bullet->params.energy)
                ? (uint8_t)(player->state.energy - bullet->params.energy)
                : 0u;
      }
      player->state.hit_time = GetTime();
      addShootIntoGameStat(stat);
      TraceLog(LOG_INFO, "[Player] HIT! health = %u", player->state.health);
    }

    node = node->next;
  }

  removeBullets(bullets);
}
