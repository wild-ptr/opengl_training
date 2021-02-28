#include "Renderable.h"
#include <glad/glad.h>
#include <assert.h>

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
}

void renderable_create(Renderable* renderable,
                       float vertices[static 1],
                       size_t vertices_size,
                       unsigned int indices[],
                       size_t indices_size,
                       Texture* textures,
                       size_t textures_size,
                       Shader* shader,
                       Matrices* mvp_matrices)
{
    if (textures_size < 10)
    {
        printf("num_of_textures cannot be more than 10!\n");
        return;
    }

    glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_DYNAMIC_DRAW);
    renderable->VAO = createAndBindVao();
    renderable->VBO = createAndBindVbo();

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

    renderable->textures = textures;
    renderable->num_textures = textures_size;
    renderable->num_vertices = vertices_size;
    renderable->shader = shader;
    renderable->matrices = mvp_matrices;

    renderable->valid = true;
}

void renderable_up_shader_uni_data(Renderable* renderable, void* uniform_calc_data)
{
    renderable->shader->uniform_calc_data = uniform_calc_data;
}

// dont use, use shader for this.
void renderable_up_mvp_matrices(Renderable* renderable, Matrices* matrices)
{
    renderable->matrices = matrices;
    shader_setUniform4mat(renderable->shader, "model_m", &matrices->model_m);
    shader_setUniform4mat(renderable->shader, "view_m", &matrices->view_m);
    shader_setUniform4mat(renderable->shader, "proj_m", &matrices->proj_m);
}

void renderable_draw(Renderable* renderable)
{
    glBindVertexArray(renderable->VAO);
    shader_use(renderable->shader);
    for(int i = 0; i < renderable->num_textures; ++i)
    {
        texture_use_texunit(&renderable->textures[i], GL_TEXTURE0 + i);
    }

    if(renderable->indexed)
    {
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderable->EBO); // VAO should handle this.
        glDrawElements(GL_TRIANGLES, renderable->num_indices, GL_UNSIGNED_INT, 0);
        return;
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, renderable->num_vertices);
        return;
    }
}
