/**
 * @file models.c
 * @brief Module for loading and managing 3D ship models using raylib.
 *
 * This module provides functionality to load multiple 3D ship models from disk,
 * store them in a doubly-linked list, manage their lifecycle, and associate
 * them with shared shader and texture resources.
 *
 * All models are expected to reside in the "assets/models" directory, and each
 * model must include both a `.obj` file and a `.png` texture with the same
 * name.
 *
 * Example model set includes "CamoStellarJet", "RedFighter", etc.
 */
#include "models.h"
#include "../utils/path.h"
#include "raylib.h"
#include "rlgl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @def MODELS
 * @brief Path to the directory containing all model assets.
 */
#define MODELS "assets/models"

/**
 * @def LIGHTS
 * @brief Path to the directory containing lighting shaders.
 */
#define LIGHTS "assets/lights"

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

/**
 * @def MODEL_OBJ_EXT
 * @brief File extension for model geometry files.
 */
#define MODEL_OBJ_EXT ".obj"

/**
 * @def MODEL_PNG_EXT
 * @brief File extension for model texture files.
 */
#define MODEL_PNG_EXT ".png"

/**
 * @brief Constructs a full file path to a model asset file.
 *
 * Combines the base model directory, model name, and file extension into a full
 * path. The returned string is heap-allocated and must be freed by the caller.
 *
 * @param filename The base name of the model (without extension).
 * @param ext File extension (e.g., ".obj", ".png").
 * @return char* Full path to the asset file, or NULL on allocation failure.
 */
char *getFilesPath(const char *filename, const char *ext) {
  char *model_path = path_join(MODELS, path_join(filename, filename));
  if (!model_path)
    return NULL;
  size_t len = strlen(model_path) + strlen(ext) + 1;
  char *result = malloc(len);
  if (!result) {
    return NULL;
  }
  snprintf(result, len, "%s%s", model_path, ext);
  free(model_path);
  return result;
}

/**
 * @brief Loads a 3D model and its texture from disk and assigns a shader.
 *
 * Loads both `.obj` and `.png` files corresponding to a model name,
 * initializes the model, binds the texture, assigns the given shader,
 * and wraps the result in a ShipModel struct.
 *
 * @param filename Name of the model (without path or extension).
 * @param shader Pointer to the shared shader used by this model.
 * @return ShipModel* Allocated model instance, or NULL on failure.
 */
ShipModel *loadShipModel(const char *filename, Shader *shader) {
  char *path_obj = getFilesPath(filename, MODEL_OBJ_EXT);
  if (!path_obj) {
    fprintf(stderr, "[ModelLoader] Fail load model: %s\n", filename);
    exit(EXIT_FAILURE);
  }
  Model model = LoadModel(path_obj);
  free(path_obj);

  char *path_texture = getFilesPath(filename, MODEL_PNG_EXT);
  if (!path_texture) {
    fprintf(stderr, "[ModelLoader] Fail load model's texture: %s\n", filename);
    exit(EXIT_FAILURE);
  }
  Texture2D texture = LoadTexture(path_texture);
  free(path_texture);
  model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
  model.materials[0].shader = *shader;
  ShipModel *ship = malloc(sizeof(ShipModel));
  if (!ship) {
    UnloadModel(model);
    UnloadTexture(texture);
    return NULL;
  }
  ship->model = model;
  ship->texture = texture;
  ship->model_name = filename;
  return ship;
}

/**
 * @brief Frees all memory and GPU resources associated with a ShipModel.
 *
 * This includes the raylib Model, Texture2D, and the ShipModel struct itself.
 *
 * @param ship Pointer to the model to destroy. Must not be NULL.
 */
void destroyShipModel(ShipModel *ship) {
  printf("[Models] model will be unload \"%s\"\n", ship->model_name);
  UnloadModel(ship->model);
  UnloadTexture(ship->texture);
  printf("[Models] model \"%s\" has been unload\n", ship->model_name);
  free(ship);
}

/**
 * @brief Allocates and initializes a new ShipModelNode.
 *
 * Wraps a loaded ShipModel in a doubly-linked list node.
 *
 * @param model_name Name of the model to load.
 * @param shader Pointer to the shared shader.
 * @param prev Pointer to the previous node in the list (can be NULL).
 * @return ShipModelNode* Allocated node or NULL on failure.
 */
ShipModelNode *newShipModelNode(const char *model_name, Shader *shader,
                                ShipModelNode *prev) {
  ShipModel *model = loadShipModel(model_name, shader);
  if (!model) {
    return NULL;
  }
  ShipModelNode *node = malloc(sizeof(ShipModelNode));
  if (!node) {
    return NULL;
  }
  node->self = model;
  node->prev = prev;
  node->next = NULL;
  return node;
}

/**
 * @brief Frees a ShipModelNode and its associated model.
 *
 * @param node Pointer to the node to destroy. Safe to pass NULL.
 */
void destroyShipModelNode(ShipModelNode *node) {
  if (!node) {
    return;
  }
  if (node->self) {
    destroyShipModel(node->self);
  }
  free(node);
}

/**
 * @brief Loads all predefined models into a ShipModelList.
 *
 * This function initializes the shader from lighting.vs/fs files,
 * iterates over a hardcoded list of model names, loads each,
 * and stores them in a doubly-linked list.
 *
 * On any failure during loading, the function will clean up all previously
 * loaded models and return NULL.
 *
 * @return ShipModelList* Pointer to a fully initialized model list, or NULL on
 * failure.
 */
ShipModelList *newShipModelList() {
  ShipModelList *models = malloc(sizeof(ShipModelList));
  if (!models) {
    return NULL;
  }
  models->length = 0;
  models->head = NULL;
  models->tail = NULL;

  const char *list[] = {MODEL_CamoStellarJet,
                        MODEL_DualStriker,
                        MODEL_GalactixRacer,
                        MODEL_InterstellarRunner,
                        MODEL_MeteorSlicer,
                        MODEL_RedFighter,
                        MODEL_StarMarineTrooper,
                        MODEL_Transtellar,
                        MODEL_UltravioletIntruder,
                        MODEL_Warship,
                        NULL};
  // Create shader
  char *vs_file = path_join(LIGHTS, "lighting.vs");
  char *fs_file = path_join(LIGHTS, "lighting.fs");

  Shader shader = LoadShader(vs_file, fs_file);

  free(vs_file);
  free(fs_file);

  for (int i = 0; list[i] != NULL; i++) {
    printf("[Models] Loading model %s\n", list[i]);
    ShipModelNode *node = newShipModelNode(list[i], &shader, models->tail);
    if (!node) {
      destroyShipModelList(models);
      return NULL;
    }
    if (!models->head) {
      models->head = node;
    }
    if (models->tail) {
      models->tail->next = node;
    }
    models->tail = node;
    models->length += 1;
    printf("Model %s has been loaded\n", list[i]);
  }
  models->shader = shader;
  return models;
}

/**
 * @brief Frees all resources associated with a ShipModelList.
 *
 * This includes all ShipModelNodes, each model and texture, and the shared
 * shader.
 *
 * @param models Pointer to the list to destroy. Safe to pass NULL.
 */
void destroyShipModelList(ShipModelList *models) {
  if (!models) {
    return;
  }
  ShipModelNode *node = models->head;
  while (node) {
    ShipModelNode *next = node->next;
    destroyShipModelNode(node);
    node = next;
  }
  models->head = models->tail = NULL;
  models->length = 0;
  UnloadShader(models->shader);
  free(models);
}
