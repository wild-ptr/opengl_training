#pragma once
#include <stdbool.h>

typedef struct Texture
{
    unsigned int id;
    bool valid;
} Texture;

bool texture_init_rgb(Texture*, const char* path, unsigned int texture_type);
bool texture_init(Texture*, const char* path);
void texture_use(Texture*);
void texture_use_texunit(const Texture*, unsigned int active_texture);
Texture* texture_zero_init(Texture*);
