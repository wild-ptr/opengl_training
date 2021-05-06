#pragma once
#include <stdlib.h>
#include <string.h>

#include "Light.h"

#define GENERIC_VECTOR_INITIAL_SIZE 40

#define LV_AT(vector, position) \
    ((vector)).data[(position)]

typedef struct LightVector
{
    size_t size;
    size_t _max_size;
    void** data;
} LightVector;

// This class shall only manage lights.
LightVector light_vector_init()
{
    LightVector vec;
    vec.size = 0;
    vec._max_size = GENERIC_VECTOR_INITIAL_SIZE;
    vec.data = calloc(sizeof(void*) * GENERIC_VECTOR_INITIAL_SIZE);
    return vec;
}

light_vector_destroy(LightVector* vec)
{
    // deallocate elements first
    for (size_t i = 0; i < vec.size; ++i)
    {
        free(data[i]);
    }

    // and the dynamic array itself
    free(data);
    vec.data = NULL
    size_t _max_size = 0;
    size_t size = 0;
}

// elem gets copied to array, so it can have temporary lifetime during the call.
lv_push_back(LightVector* vec, void* elem)
{
    enum ELightType type = light_get_type(elem);

    if(vec.size >= vec._max_size)
    {
        // reallocate array itself, not elements.
        vec.data = realloc(data, sizeof(void*) * vec._max_size * 2);
        vec._max_size *= 2;
    }

    switch(type)
    {
        case LIGHT_OMNIDIR:
            data[vec.size] = malloc(sizeof(Light));
            *data[vec.size] = (Light)(*elem);
            break;
        case LIGHT_UNIDIR:
            data[vec.size] = malloc(sizeof(DirectionalLight));
            *data[vec.size] = (DirectionalLight)(*elem);
            break;
        case LIGHT_SPOTLIGHT:
            data[vec.size] = malloc(sizeof(SpotLight));
            *data[vec.size] = (SpotLight)(*elem);
            break;
        default:
            assert(false);
            break;
    }
    ++vec.size;
}

bool lv_remove(LightVector* vec, size_t index)
{
    if(vec->size - 1 < index)
        return false;

    // back removal is almost free, nothing fancy.
    if(vec->size - 1 == index)
    {
        free(vec->data[index]);
        vec->data[index] = NULL;
        vec->size -= 1;
        return true;
    }
    else // reallocate elements to maintain continuity.
    {
        free(vec->data[index]);
        //vec->data[index] = NULL;

        size_t begin_offset = index;
        size_t elem_count_to_reloc = vec->size - index - 1;

        memmove(vec->data + begin_offset, vec->data + begin_offset + 1, elem_count_to_reloc);

        // since last element will be duplicated after memmove, we need to remove it.
        // Dont free memory as its just a duplicate pointer, not a duplicate element.
        memset(vec->data[vec->size - 1], 0, sizeof(void*));

        vec->size -= 1;

        return true;
    }
}
