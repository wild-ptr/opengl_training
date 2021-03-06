#include "Renderable.h"
#include <glad/glad.h>
#include <assert.h>
#include <string.h>

// binds Vertex Buffer Object to array buffer of opengl.
static int createAndBindVbo()
{
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    return VBO;
}

// binds Vertex Array Object.
// It stores the following:
// - vertexAttribArray and pointers
// - vertex buffer objects.
// so it basically binds VBO buffer and its attributes together.
// Easy enough.
static unsigned int createAndBindVao()
{
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    return VAO;
}

static unsigned int createAndBindEbo()
{
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    return EBO;
}

void renderable_bind(Renderable* r)
{
    if(r->valid)
    {
        glBindVertexArray(r->VAO);
        if(r->EBO) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r->EBO);
    }
}

void renderable_create(Renderable* renderable,
                       float vertices[static 1],
                       size_t vertices_size,
                       unsigned int indices[],
                       size_t indices_size,
                       Texture* textures,
                       size_t textures_size,
                       const Shader* shader)
{
    renderable_init(renderable);
    if (textures_size > 10)
    {
        printf("num_of_textures cannot be more than 10!\n");
        return;
    }

    renderable->VAO = createAndBindVao();
    renderable->VBO = createAndBindVbo();
    glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_DYNAMIC_DRAW);

    if(indices != NULL)
    {
        renderable->EBO = createAndBindEbo();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices, GL_STATIC_DRAW);
        renderable->indexed = true;
        renderable->num_indices = indices_size;
    }

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(0); // vertex attributes are disabled by default.
    glEnableVertexAttribArray(1);


    renderable->num_textures = textures_size;
    renderable->num_vertices = vertices_size;

    if(textures_size)
    {
        renderable->textures = malloc(sizeof(Texture) * textures_size);
        memcpy(renderable->textures, textures, sizeof(Texture) * textures_size);
    }

    renderable->shader = malloc(sizeof(shader));
    *renderable->shader = *shader;

    renderable->valid = true;
}

void renderable_free(Renderable* r)
{
    if(r->shader != NULL)
        free(r->shader);
    if(r->textures != NULL)
        free(r->textures);

    renderable_init(r); // set all to zero
}

void renderable_up_shader_uni_data(Renderable* renderable, void* uniform_calc_data)
{
    renderable->shader->uniform_calc_data = uniform_calc_data;
}

void renderable_draw(Renderable* renderable)
{
    glBindVertexArray(renderable->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, renderable->VBO); // possibly not needed.

    for(int i = 0; i < renderable->num_textures; ++i)
    {
        texture_use_texunit(&renderable->textures[i], GL_TEXTURE0 + i);
    }

    shader_use(renderable->shader);
    shader_calculate_uniforms(renderable->shader);

    if(renderable->indexed)
    {
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderable->EBO); // VAO should handle this.
        glDrawElements(GL_TRIANGLES, renderable->num_indices, GL_UNSIGNED_INT, 0);
        return;
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, renderable->num_vertices/(5*sizeof(float)));
        return;
    }
}

void renderable_init(Renderable* renderable)
{
    renderable->VAO = 0;
    renderable->VBO = 0;
    renderable->EBO = 0;
    renderable->textures = NULL;
    renderable->num_textures = 0;
    renderable->num_indices = 0;
    renderable->num_vertices = 0;
    renderable->shader = NULL;
    renderable->valid = false;
    renderable->indexed = false;
}
