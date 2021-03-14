#pragma once
#include <cglm/call.h>

// This structure is to be reflected in the shader.

typedef struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
} Light;

