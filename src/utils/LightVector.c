#include "utils/LightVector.h"
#include <string.h>
#include <assert.h>

#define GENERIC_VECTOR_INITIAL_SIZE 40

LightVector light_vector_init()
{
    LightVector vec;
    vec.size = 0;
    vec._max_size = GENERIC_VECTOR_INITIAL_SIZE;
    vec.data = calloc(GENERIC_VECTOR_INITIAL_SIZE, sizeof(void*));
    return vec;
}

void light_vector_destroy(LightVector* vec)
{
    // deallocate elements first
    for (size_t i = 0; i < vec->size; ++i)
    {
        free(vec->data[i]);
    }

    // and the dynamic array itself
    free(vec->data);
    vec->data = NULL;

    vec->_max_size = 0;
    vec->size = 0;
}

// elem gets copied to array, so it can have temporary lifetime during the call.
void lv_push_back(LightVector* vec, void* elem)
{
    enum ELightType type = light_get_type(elem);

    if(vec->size >= vec->_max_size)
    {
        // reallocate array itself, not elements.
        vec->data = realloc(vec->data, sizeof(void*) * vec->_max_size * 2);
        vec->_max_size *= 2;
    }

    switch(type)
    {
        case LIGHT_OMNIDIR:
            vec->data[vec->size] = malloc(sizeof(Light));
            memcpy(vec->data[vec->size], elem, sizeof(Light));
            break;
        case LIGHT_UNIDIR:
            vec->data[vec->size] = malloc(sizeof(LightDirectional));
            memcpy(vec->data[vec->size], elem, sizeof(LightDirectional));
            break;
        case LIGHT_SPOTLIGHT:
            vec->data[vec->size] = malloc(sizeof(LightSpotlight));
            memcpy(vec->data[vec->size], elem, sizeof(LightSpotlight));
            break;
        default:
            assert(false);
            break;
    }
    ++vec->size;
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
        size_t relocation_size = (vec->size - index - 1) * sizeof(void*);

        memmove(&vec->data[begin_offset], &vec->data[begin_offset] + 1, relocation_size);

        // since last element will be duplicated after memmove, we need to remove it.
        // Dont free memory as its just a duplicate pointer, not a duplicate element.
        vec->data[vec->size - 1] = NULL;
        vec->size -= 1;

        return true;
    }
}

#undef GENERIC_VECTOR_INITIAL_SIZE
