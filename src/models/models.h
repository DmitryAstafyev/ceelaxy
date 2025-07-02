#ifndef MODELS_H
#define MODELS_H

#include "raylib.h" // For Model, Texture2D, Shader
#include <stddef.h> // For size_t

/**
 * @brief Represents a loaded 3D ship model with its texture and name.
 */
typedef struct ShipModel {
  Model model;            ///< Geometry of the model loaded via raylib.
  Texture2D texture;      ///< Texture applied to the model.
  const char *model_name; ///< Name of the model (without extension or path).
} ShipModel;

/**
 * @brief Doubly linked list node containing a ship model.
 */
typedef struct ShipModelNode {
  struct ShipModelNode *next; ///< Pointer to the next node.
  struct ShipModelNode *prev; ///< Pointer to the previous node.
  ShipModel *self; ///< Pointer to the ship model stored in this node.
} ShipModelNode;

/**
 * @brief Container for a doubly linked list of ship models, including shared
 * shader.
 */
typedef struct {
  ShipModelNode *head; ///< Pointer to the first node in the list.
  ShipModelNode *tail; ///< Pointer to the last node in the list.
  Shader shader;       ///< Shared shader used by all models in the list.
  size_t length;       ///< Number of models in the list.
} ShipModelList;

/**
 * @brief Frees a ShipModelNode and its associated model.
 *
 * @param node Pointer to the node to destroy. Safe to pass NULL.
 */
void destroyShipModelNode(ShipModelNode *node);

/**
 * @brief Initializes and loads all ship models into a linked list.
 *
 * @return ShipModelList* Pointer to a newly allocated and populated model list,
 *                        or NULL on failure.
 */
ShipModelList *newShipModelList(void);

/**
 * @brief Frees all resources associated with the ship model list,
 *        including models, textures, and shader.
 *
 * @param models Pointer to the list to destroy. Safe to pass NULL.
 */
void destroyShipModelList(ShipModelList *models);

#endif // MODELS_H
