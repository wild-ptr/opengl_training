#pragma once
#include <stdbool.h>

typedef struct Texture
{
    unsigned int id;
    bool valid;
} Texture;

bool texture_init_rgb(Texture*, const char* path, unsigned int texture_type);
void texture_use(Texture*);
void texture_use_texunit(Texture*, unsigned int active_texture);
