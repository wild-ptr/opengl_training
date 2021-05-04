#pragma once
#include <stdbool.h>
#include "Shader.h"
#include "Material.h"

// I believe this should become an opaque object in the future.
// There is no point for user to touch any of that.

typedef void(*renderable_bind_attributes_fp)(void);

typedef struct Renderable
{
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    Material* materials;
    int num_materials;
    int num_indices;
    int num_vertices;
    Shader* shader;
    bool valid;
    bool indexed;
} Renderable;


void renderable_init(Renderable* renderable);

void renderable_create(Renderable* renderable,
                       renderable_bind_attributes_fp bind_fp,
                       float vertices[static 1],
                       size_t vertices_size,
                       unsigned int indices[],
                       size_t indices_size,
                       Material materials[], // deep copied
                       size_t materials_size,
                       const Shader* shader); // deep copied

void renderable_draw(Renderable* renderable);

void renderable_up_shader_uni_data(Renderable* renderable,
                                   void* uniform_calc_data);

void renderable_bind(Renderable*);

void renderable_free(Renderable*);
