#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <glad/glad.h>
#include "Shader.h"


static const int ERR_VALUE = INT_MAX;

char* dumpFileContent(const char* filepath)
{
    printf("dumping file contents : %s\n", filepath);
    FILE* f = fopen(filepath, "r");
    if (f == NULL)
    {
        printf("%s : cannot open file %s\n", __FUNCTION__, filepath);
        return '\0';
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

    char *string = malloc(fsize + 1);
    fread(string, 1, fsize, f);
    fclose(f);

    string[fsize] = '\0';
    return string;
}

static bool checkShaderCompileStatus(unsigned int vertex_shader)
{
    // check status, print errors.
    int success;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
        printf("Shader compilation error:\n%s", infoLog);
        return false;
    }
    return true;
}

static bool checkShaderLinkStatus(unsigned int shader_prog)
{
    int success;
    glGetProgramiv(shader_prog, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(shader_prog, 512, NULL, infoLog);
        printf("Shader link error:\n%s", infoLog);
        return false;
    }
    return true;
}

static unsigned int compileShader(const char* v_shader_source, int shader_type)
{
    unsigned int shader_obj;
    shader_obj = glCreateShader(shader_type);
    printf("%s:\n%s\n\n", __FUNCTION__, v_shader_source);
    glShaderSource(shader_obj, 1, &v_shader_source, NULL);
    glCompileShader(shader_obj);

    return checkShaderCompileStatus(shader_obj) ? shader_obj : ERR_VALUE;
}

bool shader_init(Shader* shader, const char* v_shader_path, const char* frag_shader_path)
{
    bool ret;
    char* v_shader_str = dumpFileContent(v_shader_path);
    char* frag_shader_str = dumpFileContent(frag_shader_path);

    int v_shader = compileShader(v_shader_str, GL_VERTEX_SHADER);
    int frag_shader = compileShader(frag_shader_str, GL_FRAGMENT_SHADER);

    if ((v_shader == ERR_VALUE) || (frag_shader == ERR_VALUE))
    {
        ret = false;
        shader->valid = false;
        goto cleanup;
    }

    shader->id = glCreateProgram();
    glAttachShader(shader->id, v_shader);
    glAttachShader(shader->id, frag_shader);
    glLinkProgram(shader->id);

    if (!checkShaderLinkStatus(shader->id))
    {
        ret = false;
        shader->valid = false;
        goto cleanup;
    }

    ret = true;
    shader->valid = true;

cleanup:
    free(v_shader_str);
    free(frag_shader_str);
    glDeleteShader(v_shader);
    glDeleteShader(frag_shader);
    return ret;
}

bool shader_init_from_sources(Shader* shader, const char* v_shader_str, const char* frag_shader_str)
{
    bool ret;
    shader->valid = false;

    int v_shader = compileShader(v_shader_str, GL_VERTEX_SHADER);
    int frag_shader = compileShader(frag_shader_str, GL_FRAGMENT_SHADER);

    if ((v_shader == ERR_VALUE) || (frag_shader == ERR_VALUE))
    {
        ret = false;
        goto cleanup;
    }

    shader->id = glCreateProgram();
    glAttachShader(shader->id, v_shader);
    glAttachShader(shader->id, frag_shader);
    glLinkProgram(shader->id);

    if (!checkShaderLinkStatus(shader->id))
    {
        ret = false;
        goto cleanup;
    }

    ret = true;
    shader->valid = true;

cleanup:
    glDeleteShader(v_shader);
    glDeleteShader(frag_shader);
    return ret;
}

void shader_setUniform4f(Shader* shader, const char* name, Float4Vec vec4)
{
    int uniform_loc = glGetUniformLocation(shader->id, name);
    glUseProgram(shader->id);
    glUniform4fv(uniform_loc, 4, (float*)&vec4);
}

void shader_setUniformf(Shader* shader, const char* name, float value)
{
    int uniform_loc = glGetUniformLocation(shader->id, name);
    glUseProgram(shader->id);
    glUniform1f(uniform_loc, value);
}

void shader_setUniformi(Shader* shader, const char* name, int value)
{
    int uniform_loc = glGetUniformLocation(shader->id, name);
    glUseProgram(shader->id);
    glUniform1i(uniform_loc, value);
}

void shader_setUniform4mat(Shader* shader, const char* name, mat4* value)
{
    int uniform_loc = glGetUniformLocation(shader->id, name);
    glUseProgram(shader->id);
    glUniformMatrix4fv(uniform_loc,1, GL_FALSE, (float*)value);
}

void shader_use(Shader* shader)
{
    if(!shader->valid)
    {
        printf("Trying to use invalid shader.\n");
        return;
    }
    glUseProgram(shader->id);
}
