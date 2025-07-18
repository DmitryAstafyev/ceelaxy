#ifndef MODELS_H
#define MODELS_H

#include "raylib.h"
#include <stddef.h> // For size_t

/**
 * @def MODEL_<Name>
 * @brief Identifiers for each built-in model (used during loading).
 */
#define MODEL_CamoStellarJet "CamoStellarJet"
#define MODEL_DualStriker "DualStriker"
#define MODEL_GalactixRacer "GalactixRacer"
#define MODEL_InterstellarRunner "InterstellarRunner"
#define MODEL_MeteorSlicer "MeteorSlicer"
#define MODEL_RedFighter "RedFighter"
#define MODEL_StarMarineTrooper "StarMarineTrooper"
#define MODEL_Transtellar "Transtellar"
#define MODEL_UltravioletIntruder "UltravioletIntruder"
#define MODEL_Warship "Warship"

typedef struct ShipBoundingBox {
  float by_x;
  float by_y;
  float by_z;
} ShipBoundingBox;

/**
 * @brief Represents a loaded 3D ship model with its texture and name.
 */
typedef struct ShipModel {
  Model model;            ///< Geometry of the model loaded via raylib.
  Texture2D texture;      ///< Texture applied to the model.
  const char *model_name; ///< Name of the model (without extension or path).
  ShipBoundingBox box;
  Model *box_model;
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
 * @brief Sets the diffuse color of the ship model's material.
 *
 * Updates the base color (diffuse map) of the first material in the model.
 * If the model pointer is NULL, the function does nothing.
 *
 * @param model Pointer to the ShipModel to modify.
 * @param color The new color to apply to the model's diffuse map.
 */
void setShipModelColor(ShipModel *model, Color color);

/**
 * @brief Frees a ShipModelNode and its associated model.
 *
 * @param node Pointer to the node to destroy. Safe to pass NULL.
 */
void destroyShipModelNode(ShipModelNode *node);

/**
 * @brief Finds a ship model in the list by its name.
 *
 * Iterates through the linked list of ship models and returns a pointer
 * to the first model whose `model_name` matches the given `name`.
 * The comparison is case-sensitive.
 *
 * @param list Pointer to the ShipModelList to search.
 * @param name Name of the model to find.
 * @return Pointer to the matching ShipModel, or NULL if not found.
 */
ShipModel *findModelInList(ShipModelList *list, char *name);

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
