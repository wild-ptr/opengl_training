#pragma once
#include <cglm/call.h>
#include <assert.h>
#include <string.h>

#ifndef LIGHT_INTERNAL_MEMSET_INIT
#define LIGHT_INTERNAL_MEMSET_INIT(LightType, TypeEnum) \
    LightType light; \
    memset(&light, 0, sizeof(LightType)); \
    light.light_type = (TypeEnum); \
    return light
#endif

enum ELightType
{
    LIGHT_OMNIDIR,
    LIGHT_UNIDIR,
    LIGHT_SPOTLIGHT,
    LIGHT_TYPE_ENUM_LENGTH
};

// safe as long as ELightType is always a first member.
inline enum ELightType light_get_type(void* light)
{
    enum ELightType* type = (enum ELightType*)(light);
    assert(*type < LIGHT_TYPE_ENUM_LENGTH);
    return *type;
}



// those structures are to be reflected in the shader.
struct AttenuationParams
{
    float constant;
    float linear;
    float quadratic;
};

typedef struct Light
{
    enum ELightType light_type;
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    struct AttenuationParams att_params;
} Light;

typedef struct DirectionalLight
{
    enum ELightType light_type;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    struct AttenuationParams att_params;
} DirectionalLight;

typedef struct SpotLight
{
    enum ELightType light_type;
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float cutoff_cosine;

    struct AttenuationParams att_params;
} SpotLight;

inline Light light_init()
{
    LIGHT_INTERNAL_MEMSET_INIT(Light, LIGHT_OMNIDIR);
}

inline DirectionalLight light_directional_init()
{
    LIGHT_INTERNAL_MEMSET_INIT(DirectionalLight, LIGHT_UNIDIR);
}

inline SpotLight light_spotlight_init()
{
    LIGHT_INTERNAL_MEMSET_INIT(SpotLight, LIGHT_SPOTLIGHT);
}

#undef LIGHT_INTERNAL_MEMSET_INIT
