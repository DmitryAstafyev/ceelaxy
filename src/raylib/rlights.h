#ifndef RLIGHTS_H
#define RLIGHTS_H

#include "raylib.h"

#define MAX_LIGHTS 4

typedef enum { LIGHT_DIRECTIONAL, LIGHT_POINT } LightType;

typedef struct {
    int enabled;
    LightType type;
    Vector3 position;
    Vector3 target;
    Color color;

    int enabledLoc;
    int typeLoc;
    int positionLoc;
    int targetLoc;
    int colorLoc;
} Light;

Light CreateLight(int type, Vector3 position, Vector3 target, Color color, Shader shader);
void UpdateLightValues(Shader shader, Light light);

#endif
