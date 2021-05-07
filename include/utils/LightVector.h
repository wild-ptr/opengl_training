#pragma once
#include <stdlib.h>
#include <string.h>

#include "Light.h"
// This can be abstracted away after into GenericVector class and this as wrapper.


#define LV_AT(vector, position) \
    ((vector)).data[(position)]

#define LV_FOR_EACH(vec, index) \
    for(size_t (index) = 0; (index) < (vec).size; ++(index))

#define LV_INIT_PUSH(vec, LightType, initializer)                           \
do                                                                          \
{                                                                           \
    if(vec.size >= vec._max_size)                                           \
    {                                                                       \
        vec.data = realloc(vec.data, sizeof(void*) * vec._max_size * 2);    \
        vec._max_size *= 2;                                                 \
    }                                                                       \
                                                                            \
    LightType temp = initializer;                                           \
                                                                            \
    vec.data[vec.size] = malloc(sizeof(LightType));                         \
    memcpy(vec.data[vec.size], &temp, sizeof(LightType));                   \
    ++vec.size;                                                             \
} while(false)

typedef struct LightVector
{
    size_t size;
    size_t _max_size;
    void** data;
} LightVector;

// This class shall only manage lights.
LightVector light_vector_init();

void light_vector_destroy(LightVector* vec);

// elem gets copied to array.
void lv_push_back(LightVector* vec, void* elem);
bool lv_remove(LightVector* vec, size_t index);
