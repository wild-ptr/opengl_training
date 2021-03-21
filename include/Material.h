#pragma once
#include <cglm/call.h>
#include "Texture.h"

// This structure is to be reflected in the shader.
// This is to be changed to be the main texture system.
typedef struct Material
{
    Texture diffuse_map;
    Texture specular_map;
    Texture normal_map;
    float shininess;
} Material;

Material* material_init(Material*,
                        const char* path, unsigned int texture_type,
                        const char*, unsigned int,
                        const char*, unsigned int);

void material_use(Material*, unsigned int texunit_offset);
