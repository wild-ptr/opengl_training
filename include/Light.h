#pragma once
#include <cglm/call.h>

enum ELightType
{
    LIGHT_OMNIDIR,
    LIGHT_UNIDIR,
    LIGHT_SPOTLIGHT,
    LIGHT_TYPE_ENUM_LENGTH
};

// those structures are to be reflected in the shader.
// Except for Attenuation params, which need to be included flatly in the shader.
struct AttenuationParams
{
    float att_constant;
    float att_linear;
    float att_quadratic;
};

struct LightColor
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

typedef struct Light
{
    enum ELightType light_type;
    vec3 position;

    struct LightColor light_color;
    struct AttenuationParams att_params;
} Light;

typedef struct LightDirectional
{
    enum ELightType light_type;
    vec3 direction;

    struct LightColor light_color;
    struct AttenuationParams att_params;
} LightDirectional;

typedef struct LightSpotlight
{
    enum ELightType light_type;
    vec3 position;
    vec3 direction;

    float inner_cone_cosine;
    float outer_cone_cosine;

    struct LightColor light_color;
    struct AttenuationParams att_params;
} LightSpotlight;

// safe as long as ELightType is always a first member.
enum ELightType light_get_type(void* light);
Light light_init();
LightDirectional light_directional_init();
LightSpotlight light_spotlight_init();
