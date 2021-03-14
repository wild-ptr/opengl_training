#pragma once
#include <cglm/call.h>

// This structure is to be reflected in the shader.

typedef struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;

    vec3 position;

} Material;
