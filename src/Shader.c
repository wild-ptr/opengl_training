#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <glad/glad.h>
#include <string.h>
#include <assert.h>
#include "Shader.h"
#include "Light.h"
#include "utils/LightVector.h"


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
    //printf("%s:\n%s\n\n", __FUNCTION__, v_shader_source);
    glShaderSource(shader_obj, 1, &v_shader_source, NULL);
    glCompileShader(shader_obj);

    return checkShaderCompileStatus(shader_obj) ? shader_obj : ERR_VALUE;
}

bool shader_init(Shader* shader, const char* v_shader_path, const char* frag_shader_path)
{
    bool ret;
    char* v_shader_str = NULL;
    char* frag_shader_str = NULL;

    if(v_shader_path)
        v_shader_str = dumpFileContent(v_shader_path);
    if(frag_shader_path)
        frag_shader_str = dumpFileContent(frag_shader_path);

    ret = shader_init_from_sources(shader, v_shader_str, frag_shader_str);

    if(v_shader_str)
        free(v_shader_str);

    if(frag_shader_path)
        free(frag_shader_str);

    return ret;
}

bool shader_init_from_sources(Shader* shader, const char* v_shader_str, const char* frag_shader_str)
{
    bool ret;
    int v_shader, frag_shader;
    shader->valid = false;

    if(v_shader_str)
    {
        v_shader = compileShader(v_shader_str, GL_VERTEX_SHADER);
        if(v_shader == ERR_VALUE)
        {
            ret = false;
            goto cleanup;
        }
    }

    if(frag_shader_str)
    {
        frag_shader = compileShader(frag_shader_str, GL_FRAGMENT_SHADER);
        if(frag_shader == ERR_VALUE)
        {
            ret = false;
            goto cleanup;
        }
    }

    shader->id = glCreateProgram();
    if(v_shader_str)
        glAttachShader(shader->id, v_shader);
    if(frag_shader_str)
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
    if(v_shader_str)
        glDeleteShader(v_shader);
    if(frag_shader_str)
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
    //shader_calculate_uniforms(shader);
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
    shader_setUniform3vec(shader, buf, light->light_color.ambient);

    strcpy(buf, uniform_name);
    strcat(buf, ".diffuse");
    shader_setUniform3vec(shader, buf, light->light_color.diffuse);

    strcpy(buf, uniform_name);
    strcat(buf, ".specular");
    shader_setUniform3vec(shader, buf, light->light_color.specular);
}

void shader_set_material(Shader* shader, const Material* material, const char* uniform_name, int offset)
{
    assert(strlen(uniform_name) < 50);
    char buf[80];

    if(material->diffuse_map.valid)
    {
        strcpy(buf, uniform_name);
        strcat(buf, ".diffuse");
        texture_use_texunit(&material->diffuse_map, GL_TEXTURE0 + offset);
        shader_setUniformi(shader, buf, 0 + offset);
    }

    if(material->specular_map.valid)
    {
        strcpy(buf, uniform_name);
        strcat(buf, ".specular");
        texture_use_texunit(&material->specular_map, GL_TEXTURE1 + offset);
        shader_setUniformi(shader, buf, 1 + offset);
    }

    if(material->specular_map.valid)
    {
        strcpy(buf, uniform_name);
        strcat(buf, ".normal");
        texture_use_texunit(&material->normal_map, GL_TEXTURE2 + offset);
        shader_setUniformi(shader, buf, 2 + offset);
    }

    strcpy(buf, uniform_name);
    strcat(buf, ".shininess");
    shader_setUniformf(shader, buf, material->shininess);
}

#ifndef SHADER_INTERNAL_ITOA
#define SHADER_INTERNAL_ITOA(bufferName, size, index) \
    snprintf((bufferName), (size), "%lu", (index))
#endif

static void set_light_attenuation_params(
    Shader* shader,
    const struct AttenuationParams* params,
    char* uniform_name)
{
    char buf[80];

    snprintf(buf, 80, "%s.att_constant", uniform_name);
    shader_setUniformf(shader, buf, params->att_constant);

    snprintf(buf, 80, "%s.att_linear", uniform_name);
    shader_setUniformf(shader, buf, params->att_linear);

    snprintf(buf, 80, "%s.att_quadratic", uniform_name);
    shader_setUniformf(shader, buf, params->att_quadratic);
}

static void set_light_color_vectors(
    Shader* shader,
    const struct LightColor* color,
    char* uniform_name)
{
    char buf[80];
    snprintf(buf, 80, "%s.ambient", uniform_name);
    shader_setUniform3vec(shader, buf, color->ambient);

    snprintf(buf, 80, "%s.diffuse", uniform_name);
    shader_setUniform3vec(shader, buf, color->diffuse);

    snprintf(buf, 80, "%s.specular", uniform_name);
    shader_setUniform3vec(shader, buf, color->specular);
}

static void set_directional_light(Shader* shader, const LightDirectional* light, size_t index)
{
    char buf[80];

    char strindex[30];
    SHADER_INTERNAL_ITOA(strindex, 30, index);

    snprintf(buf, 80, "dirLights[%s].direction", strindex);
    shader_setUniform3vec(shader, buf, light->direction);

    snprintf(buf, 80, "dirLights[%s]", strindex);
    set_light_color_vectors(shader, &light->light_color, buf);
    set_light_attenuation_params(shader, &light->att_params, buf);
}

static void set_omni_light(Shader* shader, const Light* light, size_t index)
{
    char buf[80];

    char strindex[30];
    SHADER_INTERNAL_ITOA(strindex, 30, index);

    snprintf(buf, 80, "omniLights[%s].position", strindex);
    shader_setUniform3vec(shader, buf, light->position);

    snprintf(buf, 80, "omniLights[%s]", strindex);
    set_light_color_vectors(shader, &light->light_color, buf);
    set_light_attenuation_params(shader, &light->att_params, buf);
}

static void set_spot_light(Shader* shader, const LightSpotlight* light, size_t index)
{
    char buf[80];

    char strindex[15];
    SHADER_INTERNAL_ITOA(strindex, 15, index);

    snprintf(buf, 80, "spotLights[%s].direction", strindex);
    shader_setUniform3vec(shader, buf, light->direction);

    snprintf(buf, 80, "spotLights[%s].position", strindex);
    shader_setUniform3vec(shader, buf, light->position);

    snprintf(buf, 80, "spotLights[%s].inner_cone_cosine", strindex);
    shader_setUniformf(shader, buf, light->inner_cone_cosine);

    snprintf(buf, 80, "spotLights[%s].outer_cone_cosine", strindex);
    shader_setUniformf(shader, buf, light->outer_cone_cosine);

    snprintf(buf, 80, "spotLights[%s]", strindex);
    set_light_color_vectors(shader, &light->light_color, buf);
    set_light_attenuation_params(shader, &light->att_params, buf);
}

void shader_set_lights(Shader* shader, const LightVector* vec)
{
    size_t dir_lights_index = 0;
    size_t omni_lights_index = 0;
    size_t spotlight_lights_index = 0;

    LV_FOR_EACH(*vec, i)
    {
        void* light = LV_AT(*vec, i);
        enum ELightType type = light_get_type(light);

        // Almost like std::visit! Almost.
        if(type == LIGHT_UNIDIR)
        {
            printf("Setting dir light\n");
            set_directional_light(shader, light, dir_lights_index++);
            continue;
        }
        if(type == LIGHT_OMNIDIR)
        {
            set_omni_light(shader, light, omni_lights_index++);
            continue;
        }
        if(type == LIGHT_SPOTLIGHT)
        {
            set_spot_light(shader, light, spotlight_lights_index++);
            continue;
        }
    }

    shader_setUniformi(shader, "dirLightsSize" , dir_lights_index);
    shader_setUniformi(shader, "omniLightsSize" , omni_lights_index);
    shader_setUniformi(shader, "spotLightsSize" , spotlight_lights_index);
}

#undef SHADER_INTERNAL_ITOA
