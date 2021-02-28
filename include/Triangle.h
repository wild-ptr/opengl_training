#pragma once
#include <stdbool.h>
#include <stddef.h>
#include "Vertex.h"
#include "Shader.h"

typedef struct RenderParameters
{
    bool success;
    unsigned int vao;
    unsigned int shader_program;
} RenderParameters;

void drawTriangle();

RenderParameters prepareRendering(
        float vertices[static 1],
        size_t vertices_size,
        unsigned int indices[],
        size_t indices_size,
        Shader* shader);
