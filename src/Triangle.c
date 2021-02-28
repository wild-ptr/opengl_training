#include "Triangle.h"
#include "Vertex.h"
#include "Shader.h"
#include "Texture.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/call.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>


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

static float calculateUniformColor()
{
    return sin(glfwGetTime());
}

RenderParameters prepareRendering(
        float vertices[static 1],
        size_t vertices_size,
        unsigned int indices[],
        size_t indices_size,
        Shader* shader)
{
    RenderParameters ret;

    unsigned int VAO = createAndBindVao();
    unsigned int VBO = createAndBindVbo();
    // save vertex data to created VBO into static_draw gpu memory.
    glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);

    if (indices != NULL)
    {
        unsigned int EBO = createAndBindEbo();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices, GL_STATIC_DRAW);
    }
    // now, this is tricky. We have created an VBO and bound it into GL_ARRAY_BUFFER,
    // and now we have to tell openGL how to interpret data in this buffer so it
    // can be passed to vertex shader.
    // Since vertex shader accepts vec3 at location 0 (aka first parameter)
    // we need to tell gl that location 0 will be filled with 3 values from buffer each,
    // they are floats, data not normalized (GL_FALSE), stride (alignment between
    // each value tuple, here all is packed tightly, so just 3*float is next vec3.
    // 0 is an offset from start of buffer to vertex data.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(0); // vertex attributes are disabled by default.
    glEnableVertexAttribArray(1);

    glUseProgram(shader->id);

    ret.success = true;
    ret.vao = VAO;
    ret.shader_program = shader->id;

    return ret;
}


void drawTriangle()
{
    static float vertices[] =
    {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };


    static bool once_flag = false;
    static RenderParameters render_params;
    static Shader shader;
    static Texture texture1;
    static Texture texture2;

    if(!once_flag)
    {
        //shader_init_from_sources(&shader, v_shader_src, frag_shader_src);
        shader_init(&shader, "src/shaders/SimpleVShader.glsl",
                             "src/shaders/SimpleFragShader.glsl");
        shader_use(&shader);
        render_params = prepareRendering(vertices, sizeof(vertices),
                                        NULL, 0,
                                        &shader);

        texture_init_rgb(&texture1, "assets/container.jpg", GL_RGB);
        texture_init_rgb(&texture2, "assets/awesomeface.png", GL_RGBA);

        texture_use_texunit(&texture1, GL_TEXTURE0);
        texture_use_texunit(&texture2, GL_TEXTURE1);

        // set samplers2D for correct texture units in shader prog.
        shader_setUniformi(&shader, "usedTexture", 0);
        shader_setUniformi(&shader, "usedTexture2", 1);

        once_flag = true;
        if (!render_params.success)
        {
            printf("Something went terribly wrong\n");
            return;
        }
    }


    glBindVertexArray(render_params.vao);
    shader_setUniformf(&shader, "blendFactor", sin(glfwGetTime()) * 0.2f);

    mat4 model_m = GLM_MAT4_IDENTITY_INIT;
    glmc_rotate(model_m, glm_rad(-45.0f)*(float)glfwGetTime(), (vec3){1.0f, 1.0f, 0.0f});

    mat4 view_m = GLM_MAT4_IDENTITY_INIT;
    glmc_translate(view_m, (vec3){0.0f, 0.0f, sin(glfwGetTime()) - 5.0f});

    mat4 proj_m = GLM_MAT4_IDENTITY_INIT;
    glmc_perspective(glm_rad(45.0f), 1500.0f/1200.0f, 0.1f, 100.0f, proj_m);

    shader_setUniform4mat(&shader, "model_m", &model_m);
    shader_setUniform4mat(&shader, "view_m", &view_m);
    shader_setUniform4mat(&shader, "proj_m", &proj_m);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    // draw as friangles, from VBO position 0, use 6 vertices.
}
