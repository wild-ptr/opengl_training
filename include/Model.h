#pragma once
#include <stddef.h>

#include "Renderable.h"

typedef struct Model
{
    Renderable meshes[30];
    const char* directory;
    size_t meshes_size;
} Model;

void model_load(Model* model, const char* path, Shader* shader);
void model_draw(Model* model);
