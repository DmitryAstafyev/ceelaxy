#include "rlights.h"
#include <stdio.h>

Light CreateLight(int type, Vector3 position, Vector3 target, Color color, Shader shader) {
    Light light = { 0 };

    light.enabled = true;
    light.type = type;
    light.position = position;
    light.target = target;
    light.color = color;

    char locName[32];

    int index = 0; // hardcoded for simplicity; can be extended

    sprintf(locName, "lights[%d].enabled", index);
    light.enabledLoc = GetShaderLocation(shader, locName);
    sprintf(locName, "lights[%d].type", index);
    light.typeLoc = GetShaderLocation(shader, locName);
    sprintf(locName, "lights[%d].position", index);
    light.positionLoc = GetShaderLocation(shader, locName);
    sprintf(locName, "lights[%d].target", index);
    light.targetLoc = GetShaderLocation(shader, locName);
    sprintf(locName, "lights[%d].color", index);
    light.colorLoc = GetShaderLocation(shader, locName);

    UpdateLightValues(shader, light);

    return light;
}

void UpdateLightValues(Shader shader, Light light) {
    SetShaderValue(shader, light.enabledLoc, &light.enabled, SHADER_UNIFORM_INT);
    SetShaderValue(shader, light.typeLoc, &light.type, SHADER_UNIFORM_INT);
    SetShaderValue(shader, light.positionLoc, &light.position.x, SHADER_UNIFORM_VEC3);
    SetShaderValue(shader, light.targetLoc, &light.target.x, SHADER_UNIFORM_VEC3);

    float color[4] = {
        (float)light.color.r/255.0f,
        (float)light.color.g/255.0f,
        (float)light.color.b/255.0f,
        (float)light.color.a/255.0f
    };
    SetShaderValue(shader, light.colorLoc, color, SHADER_UNIFORM_VEC4);
}
