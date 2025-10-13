# CEELAXY

## Overview

The project is a game inspired by the classic 8-bit game *Galaxy*. This is an educational project aimed at exploring the `C` programming language and the core principles of pointer management and memory allocation/control.

![Game Example](docs/assets/life.gif)

## Architecture & Design

### Structure

```
./src/
├── bullets
│   ├── bullets.c  // functions for rendering and updating bullet/projectile movement
│   ├── bullets.h
│   ├── trail.c    // functions for rendering and updating projectile flight trails
│   └── trail.h
├── game
│   ├── game.c     // main game object/loop
│   ├── game.h
│   ├── levels.c   // level configuration, including difficulty settings
│   ├── levels.h
│   ├── stat.c     // gameplay statistics tracking
│   └── stat.h
├── models
│   ├── models.c   // loads and stores 3D ship models in memory
│   └── models.h
├── movement
│   ├── movement.c // handles player ship movement
│   └── movement.h
├── parallax
│   ├── parallax.c // parallax background
│   └── parallax.h
├── raylib
│   ├── rlights.c  // utility functions for working with Raylib lighting
│   └── rlights.h
├── sprites
│   ├── sprites.c  // loads and stores sprite presets (explosions, smoke, etc.)
│   └── sprites.h
├── textures
│   ├── textures.c // loads and stores textures used for visual effects
│   └── textures.h
├── units
│   ├── bars.c     // renders status bars (energy, health)
│   ├── bars.h
│   ├── explosion.c// renders explosion effect on projectile impact
│   ├── explosion.h
│   ├── player.c   // renders the player's ship
│   ├── player.h
│   ├── unit.c     // renders enemy ships
│   └── unit.h
└── utils          // utility helpers
|   ├── debug.c
|   ├── debug.h
|   ├── path.c
|   └── path.h
└── main.c
```

### Design

The `Game` structure holds references to all modules required by the game, which are initialized upfront.

```
typedef struct Game
{
  UnitList *enemies;        /// Linked list of enemy units.
  Player *player;           /// Pointer to the player instance.
  BulletList *bullets;      /// Shared bullet registry for both player and enemies.
  ShipModelList *models;    /// List of loaded 3D models used by the game.
  GameTextures *textures;   /// Pointer to loaded game textures.
  SpriteSheetList *sprites; /// List of loaded sprites textures/models
  Camera3D camera;          /// Active 3D camera used for rendering the scene.
  Light light;              /// Scene lighting setup for shading.
  GameStat stat;            /// Game statistics (hits, misses, score, etc).
  Level level;              /// Current game level and parameters.
  ParallaxField parallax;   /// Parallax starfield background effect.
} Game;
```

#### Game loading and start

**Step 1**. Asset Loading Stages:

* Load textures
* Load models
* Load sprites

**Step 2**. Game Initialization:

* Create the player
* Spawn initial enemies
* Set initial level parameters
* Create the game statistics object
* Configure cameras
* Configure lighting
* Create the background (parallax effect)

**Step 3**. Go into main Game Loop:

* Enemies appear
* If the player eliminates all enemies, the difficulty level increases and new enemies spawn
* If the player loses, the game resets to level one

### Design Highlights

* Enemy `Unit` objects and the `Player` do **not** embed ship models; they only keep **references** to them. A dedicated `ShipModelList` module owns all loaded models and is also responsible for memory cleanup (unloading models).
* Enemy `Unit` objects and the `Player` do **not** store bullet data; they only **spawn** bullets. Actual bullet state is stored in `BulletList`, which is also responsible for trajectory updates, hit detection, and destroying bullet instances on impact or when they leave the scene bounds. *Note*: collision resolution uses an `owner` field on each `Bullet` (who spawned it).
* Each `Bullet` includes a `TrailEmitter` that handles the rendering and simulation of the projectile’s exhaust trail.
* The explosion renderer `BulletExplosion` is attached to `Unit` and `Player`, **not** to individual `Bullet`s. Since there can be many bullets, recreating `BulletExplosion` for every new bullet would be wasteful. It’s more efficient to pre-create a `BulletExplosion` for entities that can explode (enemy ships and the player).

### Gameplay Mechanics

The gameplay revolves around the following actions:

* The player can move left/right and fire straight shots toward enemies.
* The player can also move forward/backward within a limited range.
* Enemies fire at the player whenever the player is inside their **line-of-sight corridor**.

Difficulty is defined by the `Level` object and two parameter blocks:

```
typedef struct
{
  float bullet_acceleration; /// Acceleration of enemy bullets
  float bullet_init_speed;   /// Initial speed of enemy bullets
  float bullet_delay_spawn;  /// Delay between enemy bullet spawns
  float damage_life;         /// Damage dealt by enemy units
  float damage_energy;       /// Energy cost or impact of enemy units
  uint16_t count;            /// Number of enemy units in the level
  ModelId model;             /// 3D model ID for enemy units
  uint8_t max_col;           /// Maximum columns in enemy formation
  uint8_t max_ln;            /// Maximum lines in enemy formation
} LevelUnitsParameters;
```

```
typedef struct
{
  float bullet_acceleration; /// Acceleration of player bullets
  float bullet_init_speed;   /// Initial speed of player bullets
  float bullet_delay_spawn;  /// Delay between player bullet spawns
  float damage_life;         /// Damage dealt by player bullets
  float damage_energy;       /// Energy cost or impact of player bullets
} LevelPlayerParameters;
```

Key changes when leveling up:

* Enemy rate of fire increases.
* Enemy bullet damage increases.
* Initial bullet speed and acceleration increase.

An important attribute for both player and enemies is `energy`. It regulates mobility. As the player takes damage, acceleration decreases, effectively slowing movement. On each level transition, the player’s `energy` and `health` are fully restored.

### Controls

- **Left / Right**: move horizontally  
- **Up / Down**: limited forward/backward movement  
- **Space**: fire  
- **Ctrl + C**: quit  

## Build & Run

### Dependencies

The engine is built on top of `raylib`, though it should be noted that `DrawMeshInstanced` was deliberately not used-even though it would have significantly improved enemy rendering performance-due to compatibility issues on Linux systems.

#### Arch Linux
```
sudo pacman -S base-devel pkgconf raylib
```
#### Debian/Ubuntu

```
sudo apt update
sudo apt install -y build-essential pkg-config libraylib-dev
```

#### macOS (Homebrew)

```
brew install raylib pkg-config
```

### Build & Run

> **Assets**. The game loads resources from the `assets/` directory (models, textures, lights).  
> Run the binary **from the project root** so relative paths resolve correctly.

```
./make all
./ceelaxy
```

### Change resolution

Use `--resolution` or `-r` to change resoltion. User can define resolution by `width` only; `height` will be calculated automatically.

To run game in `1600 x 1200` px.

```
./ceelaxy -r 1600
```

## Debug mode

Debug mode can be enabled using the `--debug` flag. When enabled, model bounding containers are rendered to simplify visual debugging.

![Game Debug Mode Example](docs/assets/debug.gif)


## Copyrights and Resources

Spaceship models were sourced from open assets: https://maxparata.itch.io/voxel-spaceships.  
Sprites and textures were also not created by the author and were taken from open sources (for example, https://opengameart.org/content/explosion-particles-sprite-atlas and others from https://opengameart.org). 

## AI Cooperation

AI (ChatGPT) was used in the following aspects of this project:

* Implementation of helper functions for working with `raylib`, namely `src/raylib/raylib.c`, which were based on generated code.
* The game field background (parallax effect) was also developed with AI assistance, namely `src/parallax/parallax.c` .
* Bullet trails—both rendering and math—were implemented based on AI-generated code, specifically in the `src/bullets/trail.c` module.
* AI was actively used to solve mathematical and geometric tasks (e.g., computing slopes, rotations, and related calculations).
* All documentation (except for `README.md`) was generated with the built-in `Copilot` tool.
* AI was actively used when working with the `raylib` API, including selecting appropriate rendering techniques and data-processing approaches.
* The lighting configuration (all files in `assets/lights`) was fully generated by AI and was not modified by the author.

However, this project does not consist of code fully generated by AI. The modules mentioned above were modified and refactored as needed by the author.

## Known Limitations

- No instanced rendering for enemies: `DrawMeshInstanced` is not used due to GPU/driver compatibility issues on Linux; large formations may impact performance.
- Simple collision model (AABB + XZ overlap). Acceptable for arcade gameplay but not physically accurate.
- No audio 
- No game UI
- No way to set resolution or any other game settings


