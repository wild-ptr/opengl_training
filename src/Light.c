#include "Light.h"
#include <assert.h>
#include <string.h>

// internal macro
#ifndef LIGHT_INTERNAL_MEMSET_INIT
#define LIGHT_INTERNAL_MEMSET_INIT(LightType, TypeEnum) \
    LightType light; \
    memset(&light, 0, sizeof(LightType)); \
    light.light_type = (TypeEnum); \
    return light
#endif
//
// safe as long as ELightType is always a first member.
enum ELightType light_get_type(void* light)
{
    enum ELightType* type = (enum ELightType*)(light);
    assert(*type < LIGHT_TYPE_ENUM_LENGTH);
    return *type;
}

Light light_init()
{
    LIGHT_INTERNAL_MEMSET_INIT(Light, LIGHT_OMNIDIR);
}

LightDirectional light_directional_init()
{
    LIGHT_INTERNAL_MEMSET_INIT(LightDirectional, LIGHT_UNIDIR);
}

LightSpotlight light_spotlight_init()
{
    LIGHT_INTERNAL_MEMSET_INIT(LightSpotlight, LIGHT_SPOTLIGHT);
}

#undef LIGHT_INTERNAL_MEMSET_INIT
