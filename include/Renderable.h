#pragma once
#include <stdbool.h>
#include "Shader.h"
#include "Texture.h"

typedef struct Matrices
{
    mat4 model_m;
    mat4 view_m;
    mat4 proj_m;
} Matrices;

typedef struct Renderable
{
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    Texture* textures;
    int num_textures;
    int num_indices;
    int num_vertices;
    Shader* shader;
    bool valid;
    bool indexed;
    Matrices* matrices;
} Renderable;


void renderable_init(Renderable* renderable);

void renderable_create(Renderable* renderable,
                       float vertices[static 1],
                       size_t vertices_size,
                       unsigned int indices[],
                       size_t indices_size,
                       Texture textures[], // deep copied
                       size_t textures_size,
                       const Shader* shader, // deep copied
                       Matrices* mvp_matrices);

void renderable_draw(Renderable* renderable);

void renderable_up_shader_uni_data(Renderable* renderable,
                                   void* uniform_calc_data);

void renderable_free(Renderable*);
