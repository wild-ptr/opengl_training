#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <glad/glad.h>
#include <string.h>
#include <assert.h>
#include "Shader.h"
#include "Light.h"


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

    ret = shader_init_from_sources(shader, v_shader_str, frag_shader_str);

    free(v_shader_str);
    free(frag_shader_str);
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

bool shader_init_with_f_and_data(Shader* shader,
                              const char* v_shader_path,
                              const char* frag_shader_path,
                              uniform_calc_fp calculate_fun,
                              void* calc_data)
{
    bool ret;
    ret = shader_init(shader, v_shader_path, frag_shader_path);
    shader->uniform_calc_fp = calculate_fun;
    shader->uniform_calc_data = calc_data;
    return ret;
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

void shader_setUniform3mat(Shader* shader, const char* name, mat3* value)
{
    int uniform_loc = glGetUniformLocation(shader->id, name);
    glUseProgram(shader->id);
    glUniformMatrix3fv(uniform_loc,1, GL_FALSE, (float*)value);
}

void shader_setUniform3vec(Shader* shader, const char* name, const float value[])
{
    int uniform_loc = glGetUniformLocation(shader->id, name);
    glUseProgram(shader->id);
    glUniform3fv(uniform_loc, 1, &value[0]);
}


void shader_update_uniform_calc_data(Shader* shader, void* uniform_calc_data)
{
    shader->uniform_calc_data = uniform_calc_data;
    shader_calculate_uniforms(shader);
}

void shader_calculate_uniforms(Shader* shader)
{
    if(shader->uniform_calc_fp != NULL)
        shader->uniform_calc_fp(shader, shader->uniform_calc_data);
}

void shader_use(Shader* shader)
{
    if(!shader->valid)
    {
        //printf("Trying to use invalid shader.\n");
        return;
    }
    glUseProgram(shader->id);
}

// What about many light sources.
// This is not scallable and needs some array-setting functions for arrays of Lights
// But will have to do for now.
void shader_set_light(Shader* shader, const Light* light, const char* uniform_name)
{
    assert(strlen(uniform_name) < 50);
    char buf[80];

    strcpy(buf, uniform_name);
    strcat(buf, ".position");
    shader_setUniform3vec(shader, buf, light->position);

    strcpy(buf, uniform_name);
    strcat(buf, ".ambient");
    shader_setUniform3vec(shader, buf, light->ambient);

    strcpy(buf, uniform_name);
    strcat(buf, ".diffuse");
    shader_setUniform3vec(shader, buf, light->diffuse);

    strcpy(buf, uniform_name);
    strcat(buf, ".specular");
    shader_setUniform3vec(shader, buf, light->specular);
}

// @TODO hardcoded texture has to change to something more flexible,
// refactor Renderable to accept Materials from now on.
void shader_set_material(Shader* shader, const Material* material, const char* uniform_name)
{
    assert(strlen(uniform_name) < 50);
    char buf[80];

    strcpy(buf, uniform_name);
    strcat(buf, ".diffuse");
    texture_use_texunit(&material->diffuse_map, GL_TEXTURE0);
    shader_setUniformi(shader, buf, 0);

    strcpy(buf, uniform_name);
    strcat(buf, ".specular");
    texture_use_texunit(&material->specular_map, GL_TEXTURE1);
    shader_setUniformi(shader, buf, 1);

    strcpy(buf, uniform_name);
    strcat(buf, ".shininess");
    shader_setUniformf(shader, buf, material->shininess);
}
