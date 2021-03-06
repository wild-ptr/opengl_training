#pragma once
#include <stdbool.h>
#include "Shader.h"
#include "Texture.h"

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
} Renderable;


void renderable_init(Renderable* renderable);

void renderable_create(Renderable* renderable,
                       float vertices[static 1],
                       size_t vertices_size,
                       unsigned int indices[],
                       size_t indices_size,
                       Texture textures[], // deep copied
                       size_t textures_size,
                       const Shader* shader); // deep copied

void renderable_draw(Renderable* renderable);

void renderable_up_shader_uni_data(Renderable* renderable,
                                   void* uniform_calc_data);

void renderable_bind(Renderable*);

void renderable_free(Renderable*);
