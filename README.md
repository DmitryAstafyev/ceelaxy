## Ceelaxy

This is a toy project aimed at developing practical `C` programming skills as part of the HarvardX CS50x course (CS50's Introduction to Computer Science).

![Game Example](docs/assets/life.gif)

This is an educational project aimed at exploring the `C` programming language and the core principles of pointer management and memory allocation/control. However, the project is in a working state and can be fully completed. The core game logic has already been implemented, including:

- Positioning of enemy models
- Positioning of the player model
- Support for player shooting (spacebar key)
- Projectile trajectory computation and hit detection (taking into account not only the enemy ship’s position but also its rotation)

The engine is built on top of raylib, though it should be noted that `DrawMeshInstanced` was deliberately not used—even though it would have significantly improved enemy rendering performance—due to compatibility issues on Linux systems.

Debug mode can be enabled using the `--debug` flag. When enabled, model bounding containers are rendered to simplify visual debugging.

![Debug Mode Example](docs/assets/debug.gif)

The project is a game inspired by the classic 8-bit game *Galaxy*.
